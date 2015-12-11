/*
 * varispage.h
 *
 *  Created on: 2015年11月24日
 *      Author: cheese
 */

#ifndef VARISPAGE_H_
#define VARISPAGE_H_
#include <stddef.h>
VarisPage* pageCreate(int fd,long pos,long pageSize,int& _type);

struct KHead{
	size_t len_;
	long parent_;
	long sibling_;
};
struct VHead{
	size_t len_;
	long parent_;
	long sibling_;
};
struct KNode{
	long hash;
	long son;
};
struct VNode{
	long hash;
	long datapos;
};

struct KNodeWrapper{
	KNode node_;
	std::atomic<PageWrapper*> wrapper_;
};

class KCmp {
public:
	bool operator () (KNodeWrapper& kn1, KNodeWrapper& kn2) {
		return kn1.node_.hash <  kn2.node_.hash;
	}
};

class KEqu {
public:
	bool operator () (KNodeWrapper& kn1, KNodeWrapper& kn2) {
		return kn1.node_.hash == kn2.node_.hash;
	}
};

class VCmp {
public:
	bool operator () (VNode& vn1, VNode& vn2) {
		return vn1.hash < vn2.hash;
	}
};

class VEqu {
public:
	bool operator () (VNode& vn1, VNode& vn2) {
		return vn1.hash == vn2.hash;
	}
};

//struct VarisPagePair{
//	long pos;
//	VarisPage* pointer;
//};

union VarisPage{
	VarisKPage* kpage_;
	VarisVPage* vpage_;
};


//class VarisPage{
//private:
//	RWLock* rwlock_;
//	int level_;
//	char* diskContent_;
//public:
//	VarisPage(int fd,size_t pagePos);
//	~VarisPage();
//
//};
class VarisKPage{
private:
	RWLock* rwlock_;
	size_t usage_;
	size_t max_;
	int level_;
	char* diskContent_;
	PageController* parent_;
public:
	VarisKPage(int fd,char* buf);
	~VarisKPage();
	VarisKPage(int fd);
	VarisPage find(long hash);
	void insert(long hash,long leftpos,long rightpos);
private:
	void split();
	KNodeWrapper* knws_;
	void flushPage();

};

class VarisVPage{
private:
	RWLock* rwlock_;
	size_t usage_;
	size_t max_;
	int level_;
	char* diskContent_;
	VNode* vns_;
	VarisShard* shard_;
public:
	VarisVPage(int fd,long pagePos);
	VarisVPage(int fd,char* buf);
	~VarisVPage();
	long find(long hash);
	void insert(long hash,const char* key,const char* value);
	void insertAndDevide(long hash,long datapos);
private:
	void flushPage();
};



#endif /* VARISPAGE_H_ */
