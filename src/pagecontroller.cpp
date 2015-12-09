/*
 * pagecontroller.cpp
 *
 *  Created on: 2015年12月1日
 *      Author: cheese
 */

#include "pagecontroller.h"

PageController::PageController():pws_(),maplock_(),fd_(0),pageSize_(0){
}

PageController::~PageController(){

}

PageWrapper* PageController::get(long pos){
	maplock_.lock();
	auto iter = pws_.find(pos);
	if(iter != pws_.end()){
		maplock_.unlock();
		return *iter;
	}
	else{
		auto it = freelist_.begin();
		PageWrapper* wrapper = *it;
		freelist_.pop_front();
		pws_[pos] = wrapper;
		wrapper->initUsed();
		maplock_.unlock();
		wrapper->init(fd_,pos,pageSize_);
		return wrapper;
	}
}

void PageController::remove(long pos){
	maplock_.lock();
	auto it = pws_.find(pos);
	if(it == pws_.end()){
		maplock_.unlock();
		return;
	}
	PageWrapper* pw = *it;
	freelist_.push_back(pw);
	pws_.erase(it);
	maplock_.unlock();
	return;
}

