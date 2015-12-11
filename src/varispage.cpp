/*
 * varispage.cpp
 *
 *  Created on: 2015年12月2日
 *      Author: cheese
 */
#include "varispage.h"
#include "pagewrapper.h"
#include <string.h>
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

long VarisVPage::find(long hash){
	 VNode tmp;
	 tmp.hash = hash;
	 bool isEqual;
	 VNode* node = binarySearch(vns_[0],vns_[usage_-1],tmp,isEqual,VCmp(),VEqu());
	 if(!isEqual)
		 return -1;
	 else
		 return node->datapos;
}

void VarisVPage::insert(long hash,const char* key,const char* value){
	long datapos = shard_->insertToHeap(key,value);
	if(usage_ == max_){
		insertAndDevide(hash,datapos);
	}
	 VNode tmp;
	 tmp.hash = hash;
	 bool isEqual;
	 VNode* node = binarySearch(vns_[0],vns_[usage_-1],tmp,isEqual,VCmp(),VEqu());
	 memmove(node+1,node,(&vns_[usage_-1]-node+1)*sizeof(VNode));
	 node->hash = hash;
	 node->datapos = hash;
	 flushPage();


}

void VarisVPage::insertAndDevide(long hash,long datapos){
	size_t middle = usage_/2;
	VNode tvn;
	VNode tmp;
	tmp.hash = hash;
	bool isEqual;
	VNode* node = binarySearch(vns_[0],vns_[usage_-1],tmp,isEqual,VCmp(),VEqu());
	if(node == &vns_[usage_-1]){
		tvn.hash = hash;
		tvn.datapos = datapos;
	}
	else {
			tvn = vns_[usage_-1];
	}
	memmove(node + 1, node , (&vns_[usage_-1] - node > 1 ? &vns_[usage_-1] - node - 1 : 0) * sizeof(VNode));
	if(node < &vns_[usage_-1]) {
			node->hash = hash;
			node->datapos = datapos;
	}
	TmpVPage tmpvpage;
}

