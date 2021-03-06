#ifndef _BASE_MUTEX_H_
#define _BASE_MUTEX_H_

#include <pthread.h>
#include <boost/noncopyable.hpp>

class MutexLock : boost::noncopyable {
public:
	MutexLock() {
		pthread_mutex_init(&_mutex, NULL);
	}
	~MutexLock() {
		pthread_mutex_destroy(&_mutex);
	}
	void lock() {
		pthread_mutex_lock(&_mutex);
	}
	void unlock() {
		pthread_mutex_unlock(&_mutex);
	}	
    
    pthread_mutex_t* getPthreadMutex() {
        return &_mutex;
    }    
	
private:
	pthread_mutex_t _mutex;
};

class MutexLockGuard : boost::noncopyable {
public:
	explicit MutexLockGuard(MutexLock& mutex)
	: mutex_(mutex) {
	mutex_.lock();
	}

	~MutexLockGuard() {
	mutex_.unlock();
	}

private:
	MutexLock& mutex_;
};

#endif
