/*
 * pagewrapper.cpp
 *
 *  Created on: 2015年12月1日
 *      Author: cheese
 */
#include <stddef.h>
#include "pagewrapper.h"
PageWrapper::PageWrapper():page_(NULL),type_(UNINITIAL),using_(0),lock_(),pos_(-1){

}

bool PageWrapper::initUsed(){
	lock_.lock();
	if(using_.load()!=0){
		lock_.unlock();
		return false;
	}
	using_++;
	lock_.unlock();
	return true;
}
VarisPage* PageWrapper::get(int pos){
	lock_.lock();
	if(pos_.load() != pos){
		lock_.unlock();
		return NULL;
	}
	if(page_ == NULL){
		lock_.unlock();
		return NULL;
	}
	using_++;
	lock_.unlock();
	return page_;

}

void PageWrapper::release(){
	lock_.lock();
	using_--;
	if(using_ == 0){
		delete page_;
		page_ = NULL;
	}
	lock_.unlock();
}

void PageWrapper::init(int fd,long pos,size_t pageSize){
	lock_.lock();
	if(page_ != NULL){
		lock_.unlock();
		return;
	}
	int type;
	page_ = pageCreate(fd,pos,pageSize,type);
	type_ = type;
	pos_ = pos;
}

long PageWrapper::getPos(){
	return pos_.load();
}

