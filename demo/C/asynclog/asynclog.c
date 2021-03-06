#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "asynclog.h"
#include "logfile.h"

static asynclog_t *aslog = NULL;


void _log_append(const char* logline, int len)
{
	if (arbuffer_avail(&aslog->currentBuffer_) > len) {
		arbuffer_append(&aslog->currentBuffer_, logline, len);	/* 长度能写，就直接加入到当前缓冲 */
	}else{
		arraybufferalign_add_tail(&aslog->buffers_, &aslog->currentBuffer_); /* 长度不够则直接将当前缓存给弄进一个缓冲队列buffers_ */

		if(arbuffer_usable(&aslog->nextBuffer_)) {
			arbuffer_move(&aslog->currentBuffer_, &aslog->nextBuffer_);
		}else {
			arbuffer_init(&aslog->currentBuffer_, ASYNCLOG_BUFMAX);	
		}

		arbuffer_append(&aslog->currentBuffer_, logline, len);
		condition_notify(aslog->cond_);
	}
}

#define STATIC_BUFFER_MAX 4096
void log_append(char *fmt, ...)
{
	pthread_mutex_lock(&aslog->mutex_);

	va_list ap;
	va_start(ap, fmt);
	int n = vsnprintf(aslog->logline, ASYNCLOG_BUFMAX - 1, fmt, ap);
	va_end(ap);

	if(n > -1) 
		_log_append(aslog->logline, n);

	
	pthread_mutex_unlock(&aslog->mutex_);
}

void *asynclog_threadFunc(void *p)
{
	int i = 0;
	//assert(aslog->running_ == true);
	logfile_t *output;  
	arraybuffers_t newBuffer1;
	arraybuffers_t newBuffer2;
	arraybufferalign_t buffersToWrite;

	buffersToWrite.cout = 0;
	arbuffer_init(&newBuffer1, ASYNCLOG_BUFMAX);
	arbuffer_init(&newBuffer2, ASYNCLOG_BUFMAX);
	arraybufferalign_init(&buffersToWrite, MAXBUFFER + 1);

	output = logfile_init(aslog->basename_, aslog->rollSize_, 0, aslog->flushInterval_);

	countdownlatch_countdown(aslog->latch_);
	while (aslog->running_) {
		//assert(newBuffer1 && newBuffer1->length() == 0);
		//assert(newBuffer2 && newBuffer2->length() == 0);
		//assert(buffersToWrite.empty());

		{
			pthread_mutex_lock(&aslog->mutex_);
			if (aslog->buffers_.cout <= 0)  { // unusual usage!
				/* 如果缓冲队列为空，则等待最多3秒时间或者被append唤醒来继续下一步操作 */
				condition_waitForSeconds(aslog->cond_, aslog->flushInterval_);
			}
			arraybufferalign_add_tail(&aslog->buffers_, &aslog->currentBuffer_); /* 不管有没有数据都讲currentBuffer_的数据加入队列 */
			arraybufferalign_move(&buffersToWrite, &aslog->buffers_);		/* 临界的buffers_被交换为本地的buffters */
			arbuffer_move(&aslog->currentBuffer_, &newBuffer1);		/* 将本地的buffer替换当前缓冲，应该是为了append快速结束 */
			if (!arbuffer_usable(&aslog->nextBuffer_)) {
				arbuffer_move(&aslog->nextBuffer_, &newBuffer2);		/* 将本地的buffer2替换备用缓冲，应该是为了append快速结束 */
			}
			pthread_mutex_unlock(&aslog->mutex_);
		}

    	//assert(!buffersToWrite.empty());

		if (buffersToWrite.cout > MAXBUFFER){
			//超过MAXBUFFER的可能是日志产生方出问题了，直接只保留前面2个
			char buf[256];
			snprintf(buf, sizeof(buf), "Dropped log messages at %zu, %d larger buffers\n",
			time(NULL),
			MAXBUFFER-1);
			fputs(buf, stderr);
			logfile_append(output, buf, strlen(buf));

			arraybufferalign_erase(&buffersToWrite, 2, MAXBUFFER+1);
		}

		for (i = 0; i < buffersToWrite.cout; ++i) {
			// FIXME: use unbuffered stdio FILE ? or use ::writev ?
			logfile_append(output, buffersToWrite.buffers_[i].buffer, 
				arbuffer_used(&buffersToWrite.buffers_[i])); /* 将数据缓冲进文件 */
		}

		if (buffersToWrite.cout > 2){
			// drop non-bzero-ed buffers, avoid trashing
			/* 在短时间内大量的append操作产生的多个队列只保留2个就可以了，用来分配给本地buffer1和buffer2 */
			arraybufferalign_erase(&buffersToWrite, 2, MAXBUFFER+1);
		}

		if (!arbuffer_usable(&newBuffer1)) {
			//assert(!buffersToWrite.empty());
			arbuffer_move(&newBuffer1, &buffersToWrite.buffers_[buffersToWrite.cout-1]); /* 保留的第一块buffer，分给本地buffer1 */
			arbuffer_reset(&newBuffer1);
			buffersToWrite.cout--;
		}

		if (!arbuffer_usable(&newBuffer2)) {
			//assert(!buffersToWrite.empty());
			arbuffer_move(&newBuffer2, &buffersToWrite.buffers_[buffersToWrite.cout-1]);
			arbuffer_reset(&newBuffer2);						/* 保留的第一块buffer，分给本地buffer2 */
			buffersToWrite.cout--;
		}

		arraybufferalign_clear(&buffersToWrite);			/* 有可能nextBuffer_不是空的，所以newBuffers没有赋值给nextBuffer_，所以这里还多了一块，需要清除掉 */
		logfile_flush(output);
	}
	logfile_flush(output);
	logfile_free(output);
	arraybufferalign_clear(&buffersToWrite);
	arbuffer_clear(&newBuffer1);
	arbuffer_clear(&newBuffer2);
	return NULL;
}



void aslog_setbasename(char *basename)
{
	if(basename) {
		memcpy(aslog->basename_, basename, strlen(basename));
	}else {
		strcpy(aslog->basename_, "asynclog");
	}
}

void asynclog_start()
{
  	aslog->running_ = 1;
	pthread_create(&aslog->pid, NULL, asynclog_threadFunc, NULL);
  	countdownlatch_wait(aslog->latch_);
}

void asynclog_stop()
{
	aslog->running_ = 0;
	condition_notify(aslog->cond_);
	pthread_join(aslog->pid, NULL);
}

void asynclog_def_init(char *basename, size_t rollSize, int flushInterval)
{
	aslog = calloc(1, sizeof(asynclog_t));	
	if(aslog) {
		aslog->running_ = 0;
		aslog->flushInterval_ = flushInterval;
		aslog->rollSize_ = rollSize;
		aslog_setbasename(basename);

		pthread_mutex_init(&aslog->mutex_, NULL);
		aslog->latch_ = countdownlatch_init(&aslog->mutex_, 1);
		aslog->cond_ = condition_init(&aslog->mutex_);
		arbuffer_init(&aslog->currentBuffer_, ASYNCLOG_BUFMAX);
		arbuffer_init(&aslog->nextBuffer_, ASYNCLOG_BUFMAX);
		arraybufferalign_init(&aslog->buffers_, MAXBUFFER + 1);
	}
}

void asynclog_init(char *basename)
{
	asynclog_def_init(basename, ASYNCLOG_KROLLSIZE, ASYNCLOG_FLUSHTIME);
}

