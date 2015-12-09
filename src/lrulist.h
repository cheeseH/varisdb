/*
 * lrulist.h
 *
 *  Created on: 2015年12月1日
 *      Author: cheese
 */

#ifndef LRULIST_H_
#define LRULIST_H_

#define KPAGE 0
#define VPAGE 1
//class PageWrapper{
//private:
//	VarisPage* page_;
//	int type;
//	std::atomic<int> using_;
//	RWLock* lock_;
//public:
//	VarisPage* get();
//	void release();
//};

struct ListNode{
	std::mutex lock_;
	PageWrapper* wrapper_;
	ListNode* next_;
	ListNode* last_;
};
//struct LruList{
//	std::atomic<ListNode*> head_;
//	std::atomic<ListNode*> tail_;
//};

class LruList{
private:
	ListNode* head_;
	ListNode* tail_;
	size_t maxSize_;
	size_t currentSize_;
	std::mutex hlock_,tlock_;
public:
	LruList(size_t maxSize,PageController* parent);
	~LruList();
	void up(ListNode* node);
	void up(PageWrapper* wrapper);
private:
	void eliminate();
	PageController* parent_;
};



#endif /* LRULIST_H_ */
