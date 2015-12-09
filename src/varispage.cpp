/*
 * varispage.cpp
 *
 *  Created on: 2015年12月2日
 *      Author: cheese
 */
#include "varispage.h"
#include "pagewrapper.h"
#include <unistd.h>
VarisPage* pageCreate(int fd,long pos,long pageSize,int& _type){
	VarisPage* page = new VarisPage();
	char* buffer = new char[pageSize];
	size_t readed = 0;
	size_t begin = pos*pageSize;
	while(readed < pageSize){
		read(fd,buffer+readed,pageSize-readed);
	}
	int type = (int)buffer[0];
	_type = type;
	if(type == VPAGE){
		VarisVPage* vpage = new VarisVPage(fd,buffer);
		page->vpage_ = vpage;
		return page;
	}
	VarisKPage* kpage = new VarisKPage(fd,buffer);
	page->kpage_ = kpage;
	return page;
}


VarisPage VarisKPage::find(long hash){
	KNodeWrapper tmp;
	tmp.node_.hash = hash;
	bool isEqual;
	KNodeWrapper* knw = binarySearch(knws_[0],knws_[usage_-1],tmp,isEqual,KCmp(),KEqu());
	if(isEqual == false){
		knw--;
	}
	bool r = true;
	while(r){
		if(knw->wrapper_.load() == NULL){
			PageWrapper* tmp = NULL;
			PageWrapper* pw = parent_->get(knw->node_.son);
			r = !knw->wrapper_.compare_exchange_weak(tmp,pw);
			if(r)
				continue;
		}
		PageWrapper* pw = knw->wrapper_.load();
		VarisPage* page = pw->get(knw->node_.son);
		if(page == NULL)
			r = true;
		else{
			return page;
		}
	}


}


