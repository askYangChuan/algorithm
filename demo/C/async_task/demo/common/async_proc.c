#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "async_proc.h"


/*
��ͨд������������(����)
*/
void async_proc_append_stream(asyncProc_t *p, async_stream_t *buffer)
{
    int i = 0;
	pthread_mutex_lock(&p->mutex_);

    for(i = 0; i < buffer->cout; i++) {
        flex_append(p->currentBuffer_, buffer->data[i], buffer->len[i]);  //ֱ��д��ȥ,һ��buffer����̫��
    }

    if(readableBytes(p->currentBuffer_) >= ASYNCPROC_BUFMAX) {
        //�����滻��һ��������
        alignflexbuffer_add_tail(&p->buffers_, p->currentBuffer_);  /* ���뻺�� */
        
		if(flex_buffer_usable(p->nextBuffer_)) {
			flex_buffer_move(p->currentBuffer_, p->nextBuffer_, 0);   /* �ѵ�calloc��ֱ�Ӵ��Ѿ�����õ�nextBuffer_�����滻 */
		}else {
		    //û����ֱ�����·���һ��
		    flex_buffer_recalloc(p->currentBuffer_, ASYNCPROC_BUFMAX);
		}

        condition_notify(p->cond_);
    }

	pthread_mutex_unlock(&p->mutex_);
}

/*
��ͨд����(����)
*/
void async_proc_append(asyncProc_t *p, char* data, int len)
{
	pthread_mutex_lock(&p->mutex_);
    flex_append(p->currentBuffer_, data, len);  //ֱ��д��ȥ,һ��buffer����̫��

    if(readableBytes(p->currentBuffer_) >= ASYNCPROC_BUFMAX) {
        //�����滻��һ��������
        alignflexbuffer_add_tail(&p->buffers_, p->currentBuffer_);  /* ���뻺�� */
        
		if(flex_buffer_usable(p->nextBuffer_)) {
			flex_buffer_move(p->currentBuffer_, p->nextBuffer_, 0);   /* �ѵ�calloc��ֱ�Ӵ��Ѿ�����õ�nextBuffer_�����滻 */
		}else {
		    //û����ֱ�����·���һ��
		    flex_buffer_recalloc(p->currentBuffer_, ASYNCPROC_BUFMAX);
		}

        condition_notify(p->cond_);
    }

	pthread_mutex_unlock(&p->mutex_);
}

void *async_proc_threadFunc(void *args)
{
    asyncProc_t *p = (asyncProc_t*)args;
	int i = 0;
	//assert(aslog->running_ == true);
	flex_buffer_t *newBuffer1;
	flex_buffer_t *newBuffer2;
	alignflexbuffer_t buffersToWrite;
    flex_buffer_t *cur;

	buffersToWrite.cout = 0;
    newBuffer1 = flex_buffer_init(ASYNCPROC_BUFMAX);
    newBuffer2 = flex_buffer_init(ASYNCPROC_BUFMAX);
	alignflexbuffer_init(&buffersToWrite, ASYNCPROC_MAXBUFFER + 1);

	countdownlatch_countdown(p->latch_);
	while (p->running_) {
		//assert(newBuffer1 && newBuffer1->length() == 0);
		//assert(newBuffer2 && newBuffer2->length() == 0);
		//assert(buffersToWrite.empty());

		{
			pthread_mutex_lock(&p->mutex_);
			if (p->buffers_.cout <= 0)  { // unusual usage!
				/* ����������Ϊ�գ���ȴ����3��ʱ����߱�append������������һ������ */
				condition_waitForSeconds(p->cond_, p->flushInterval_);
			}
			alignflexbuffer_add_tail(&p->buffers_, p->currentBuffer_); /* ������û�����ݶ���currentBuffer_�����ݼ�����У��п��ܻ����һ���յĶ��� */
			alignflexbuffer_move(&buffersToWrite, &p->buffers_);		/* �ٽ��buffers_������Ϊ���ص�buffters */
			flex_buffer_move(p->currentBuffer_, newBuffer1, 0);		/* �����ص�buffer�滻��ǰ���壬Ӧ����Ϊ��append���ٽ��� */
			if (!flex_buffer_usable(p->nextBuffer_)) {
				flex_buffer_move(p->nextBuffer_, newBuffer2, 0);		/* �����ص�buffer2�滻���û��壬Ӧ����Ϊ��append���ٽ��� */
			}
			pthread_mutex_unlock(&p->mutex_);
		}

    	//assert(!buffersToWrite.empty());

		if (buffersToWrite.cout > ASYNCPROC_MAXBUFFER){
			//����MAXBUFFER�Ŀ����������߳������ˣ�ֱ��ֻ����ǰ��2��
			alignflexbuffer_erase(&buffersToWrite, 2, ASYNCPROC_MAXBUFFER+1);
		}
		for (i = 0; i < buffersToWrite.cout; ++i) {
			// FIXME: use unbuffered stdio FILE ? or use ::writev ?
			cur = &buffersToWrite.buffers_[i];
			p->proc(p, flex_peek(cur), readableBytes(cur));
		}

		if (buffersToWrite.cout > 2){
			// drop non-bzero-ed buffers, avoid trashing
			/* �ڶ�ʱ���ڴ�����append���������Ķ������ֻ����2���Ϳ����ˣ��������������buffer1��buffer2 */
			alignflexbuffer_erase(&buffersToWrite, 2, ASYNCPROC_MAXBUFFER+1);
		}

		if (!flex_buffer_usable(newBuffer1)) {
			//assert(!buffersToWrite.empty());
			flex_buffer_move(newBuffer1, &buffersToWrite.buffers_[buffersToWrite.cout-1], 0); /* �����ĵ�һ��buffer���ָ�����buffer1 */
			flex_retrieveAll(newBuffer1);
			buffersToWrite.cout--;
		}

		if (!flex_buffer_usable(newBuffer2)) {
			//assert(!buffersToWrite.empty());
			flex_buffer_move(newBuffer2, &buffersToWrite.buffers_[buffersToWrite.cout-1], 0);
			flex_retrieveAll(newBuffer2);						/* �����ĵ�һ��buffer���ָ�����buffer2 */
			buffersToWrite.cout--;
		}

		alignflexbuffer_clear(&buffersToWrite);			/* �п���nextBuffer_���ǿյģ�����newBuffersû�и�ֵ��nextBuffer_���������ﻹ����һ�飬��Ҫ����� */
	}
	alignflexbuffer_clear(&buffersToWrite);
    flex_buffer_free(newBuffer1);
    flex_buffer_free(newBuffer2);
	return NULL;
}

void async_proc_start(asyncProc_t *p)
{
  	p->running_ = 1;
	pthread_create(&p->pid, NULL, async_proc_threadFunc, p);
  	countdownlatch_wait(p->latch_);
}

void async_proc_stop(asyncProc_t *p)
{
	p->running_ = 0;
	condition_notify(p->cond_);
	pthread_join(p->pid, NULL);
}

void async_proc_free(asyncProc_t *p)
{
    pthread_mutex_destroy(&p->mutex_);
    if(p->latch_) {
        countdownlatch_free(p->latch_);
    }
    if(p->cond_) {
        condition_free(p->cond_);
    }
    flex_buffer_free(p->currentBuffer_);
    flex_buffer_free(p->nextBuffer_);
    alignflexbuffer_free(&p->buffers_);
    free(p);
}

asyncProc_t *async_proc_def_init(asyncProc_func_t f, void *data, int flushInterval_)
{
	asyncProc_t *p = calloc(1, sizeof(asyncProc_t));	
	if(p) {
		p->running_ = 0;
		p->flushInterval_ = flushInterval_;
        p->proc = f;
        p->data = data;
    
		pthread_mutex_init(&p->mutex_, NULL);
		p->latch_ = countdownlatch_init(&p->mutex_, 1);
		p->cond_ = condition_init(&p->mutex_);
        p->currentBuffer_ = flex_buffer_init(ASYNCPROC_BUFMAX);
        p->nextBuffer_ = flex_buffer_init(ASYNCPROC_BUFMAX);
		alignflexbuffer_init(&p->buffers_, ASYNCPROC_MAXBUFFER + 1);
	}

    return p;
}

asyncProc_t *async_proc_init(asyncProc_func_t f, void *data)
{
	return async_proc_def_init(f, data, ASYNCPROC_FLUSHTIME);
}
