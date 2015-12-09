/*
 * rwlock.h
 *
 *  Created on: 2015年11月25日
 *      Author: cheese
 */

#ifndef RWLOCK_H_
#define RWLOCK_H_


#include <mutex>
#include <atomic>

class RWLock
{
	std::mutex mtx1, mtx2, mtx3;
	std::atomic<size_t> rc;
public:

	void RLock() {
		mtx3.lock();
		mtx1.lock();
		mtx3.unlock();
		if(rc == 0)
			mtx2.lock();
		++rc;
		mtx1.unlock();
	}

	void RUnlock() {
		mtx1.lock();
		--rc;
		if(rc == 0)
			mtx2.unlock();
		mtx1.unlock();
	}

	void WLock() {
		mtx3.lock();
		mtx2.lock();
		mtx3.unlock();
	}

	void WUnlock() {
		mtx2.unlock();
	}
};

class AutoLock{
std::mutex mutex;
public:
	AutoLock(){
		mutex.lock();
	}
	~AutoLock(){
		mutex.unlock();
	}

};



#endif /* RWLOCK_H_ */
