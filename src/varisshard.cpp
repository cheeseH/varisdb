/*
 * varisshard.cpp
 *
 *  Created on: 2015年11月26日
 *      Author: cheese
 */
#include "varisshard.h"
#include <stddef.h>

long hashCode(const char* key);

VarisShard::VarisShard():parent_(NULL),id_(0),lrulist_(NULL),level_(0),root_(0){};

void VarisShard::init(VarisDB* parent,int id){
	parent_ = parent;
	id_ = id;
	//init lrulist

	//打开三个文件

	//读入head

	//读入root

	//init完成
}

VarisShard::~VarisShard(){
	delete(lrulist_);
}

const char* VarisShard::get(const char* key,long hash){
		if(level_ == 1){
			const char* value;
			long datapos = root_->vpage_->find(hash);
			if(datapos == -1)
				return NULL;
			else
				value = getDataInContent(datapos,key);
			return value;
		}
		else{
			int i = 1;
			VarisKPage* kpage = root_->kpage_;
			while(i<(level_-1)){
				kpage = (kpage->find(hash)).kpage_;
				if(kpage == NULL)
					return NULL;
			}
			VarisVPage* vpage = (kpage->find(hash)).vpage_;
			long datapos = vpage->find(hash);
			if(datapos == -1)
				return NULL;
			else
				return getDataInContent(datapos,key);
		}
}

const char* VarisShard::remove(const char* key,long hash){
			if(level_ == 1){
				const char* value;
				long datapos = root_->vpage_->find(hash);
				if(datapos == -1)
					return NULL;
				else
					value = getDataInContent(datapos,key);
				return value;
			}
			else{
				int i = 1;
				VarisKPage* kpage = root_->kpage_;
				while(i<(level_-1)){
					kpage = (kpage->find(hash)).kpage_;
					if(kpage == NULL)
						return NULL;
				}
				VarisVPage* vpage = (kpage->find(hash)).vpage_;
				long datapos = vpage->find(hash);
				if(datapos == -1)
					return NULL;
				else
					return getDataInContent(datapos,key);
			}
}



