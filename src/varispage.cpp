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
#include <limits.h>
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
	VHead vhead;
	vhead.parent_ = head_->parent_;
	VNode* ns = new VNode[max_];
	size_t middle = usage_/2;
	memcpy(ns, &vns_[middle], (usage_ - middle) * sizeof(VNode));
	ns[usage_-middle] = tvn;
	vhead.len_ = usage_-middle+1;
	usage_ = middle;
	long parent = head_->parent_;
	long pos = shard_->allocate();
	vhead.sibling_ = head_->sibling_;
	head_->sibling_ = pos;
	tmpvpage.head = vhead;
	tmpvpage.vnws_ = ns;
	KNode kn;
	kn.hash = hash;
	kn.son = pos;

	//写到内存
	//先判断是否为最顶点
	if(parent_ == NULL && head_->parent_ == 0){
		KHead kh;
		kh.len_ = 2;
		kh.parent_ = 0;
		KNode* nkn = new KNode[VarisKPage::MAX];
		nkn->son = shard_->getRootPos();
		nkn->hash = LONG_MIN;
		nkn++;
		*nkn = *kn;
		long pos = shard_->allocate();
		shard_->changeRoot(pos);
		shard_->levelChanged();
		return;
	}
	VarisKPage* parentPage = (parent_->get(head_->parent_))->kpage_;
	parentPage->insert(&kn);
	parent_->release(&parentPage);


}



void VarisKPage::insert(KNode* knode){
//		loadPage(pos);
		KHead* kh = (KHead*) getBuffer(0);
		bool isequal;
		KNode* kn = binarySearch(getKNode(0), getKNode(kh->len), *node, isequal, KCmp(), KEqu());
		size_t offset = ((size_t) kn - (size_t) getKNode(0)) / sizeof(KNode);
		if(kh->len < maxKNum) {
			memmove(kn + 1, kn, (kh->len - offset) * sizeof(KNode));
			*kn = *node;
			++kh->len;
			flushPage(0);
		} else {
			KHead* kh2 = (KHead*) getBuffer(1);
			KNode tkn;
			if(offset == kh->len) {
				tkn = *node;
			} else {
				tkn = *getKNode(kh->len - 1);
			}
			size_t middle = kh->len / 2;
			memmove(getKNode(offset + 1), getKNode(offset), (kh->len - offset > 1 ? kh->len - offset - 1 : 0) * sizeof(KNode));
			if(offset < kh->len) {
				*kn = *node;
			}
			memcpy(getKNode(0, getBuffer(1)), getKNode(middle), (kh->len - middle) * sizeof(KNode));
			*getKNode(kh->len - middle, getBuffer(1)) = tkn;
			kh2->len = kh->len - middle + 1;
			kh->len = middle;

			kh2->parent = kh->parent;
			long parent = kh->parent;

			node->hash = getKNode(0, getBuffer(1))->hash;

			flushPage(0);
			long npx = appendPage(getBuffer(1));

			node->son = npx;
			insertInnerNode(node, parent);
		}
}

