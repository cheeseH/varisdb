/*
 * lrulist.cpp
 *
 *  Created on: 2015年12月2日
 *      Author: cheese
 */

#include "lrulist.h"
#include <stddef.h>
LruList::LruList(size_t maxSize,PageController* parent):head_(NULL),tail_(NULL),maxSize_(maxSize),hlock_(),tlock_(),currentSize_(0),parent_(parent){

}

LruList::~LruList(){
	//删除list里的东西
}

void LruList::up(ListNode* node){
	bool getAll = false;
	std::mutex *nlock,*nexlock,*laslock,*headlock;
	while(getAll == false){
		if(!hlock_.try_lock()){
			pthread_yield();
			continue;
		}
		if(!node->lock_.try_lock()){
			hlock_.unlock();
			pthread_yield();
			continue;
		}
		nlock = &(node->lock_);
		if(!head_->lock_.try_lock()){
			hlock_.unlock();
			node->lock_.unlock();
			pthread_yield();
			continue;
		}
		headlock = &(head_->lock_);
		if(!node->last_->lock_.try_lock()){
			hlock_.unlock();
			node->lock_.unlock();
			head_->lock_.unlock();
			pthread_yield();
			continue;
		}
		laslock = &(node->last_->lock_);
		if(!node->next_->lock_.try_lock()){
			hlock_.unlock();
			node->lock_.unlock();
			head_->lock_.unlock();
			node->last_->lock_.unlock();
			pthread_yield();
			continue;
		}
		nexlock = &(node->next_->lock_);
		getAll=true;
	}
	if(node->last_ != NULL)
		node->last_->next_ = node->next_;
	if(node->next_!=NULL)
		node->next_->last_ = node->last_;
	node->next_ = head_;
	head_->last_ = node;
	currentSize_++;
	hlock_.unlock();
	nexlock->unlock();
	laslock->unlock();
	headlock->unlock();
	nlock->unlock();
	return;
}

void LruList::up(PageWrapper* wrapper){
	ListNode* node = new ListNode();
	node->wrapper_ = wrapper;
	std::mutex* headlock;
	bool getAll = false;
	hlock_.lock();
	if(head_ == NULL){
		tlock_.lock();
		head_ = tail_ = node;
		hlock_.unlock();
		tlock_.unlock();
		return;
	}
	head_->lock_.lock();
	headlock = &(head_->lock_);
	node->next_ = head_;
	head_->last_ = node;
	head_ = node;
	if(currentSize_<maxSize_){
		currentSize_++;
		hlock_.unlock();
		headlock->unlock();
	}else{
		hlock_.unlock();
		headlock->unlock();
		eliminate();
	}

}

void LruList::eliminate(){
	bool getAll = false;
	std::mutex* taillock,*laslock;
	while(!getAll){
		if(!tlock_.try_lock())
			continue;
		if(!tail_->lock_.try_lock()){
			tlock_.unlock();
			continue;
		}
		taillock = &(tail_->lock_);
		if(!tail_->last_->lock_.try_lock()){
			tlock_.unlock();
			tail_->lock_.unlock();
			continue;
		}
		laslock = tail_->last_->lock_;
		getAll = true;

	}
	tail_->last_->next_ = NULL;
	ListNode* node = tail_;
	tail_ = tail_->last_;
	tlock_.unlock();
	laslock->unlock();
	parent_->remove(node->wrapper_->getPos());
	return;
}


