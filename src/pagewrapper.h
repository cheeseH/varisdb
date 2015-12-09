/*
 * pagewrapper.h
 *
 *  Created on: 2015年12月1日
 *      Author: cheese
 */

#ifndef PAGEWRAPPER_H_
#define PAGEWRAPPER_H_

#define UNINITIAL 0
#define KPAGE 1
#define VPAGE 2
class PageWrapper{
private:
	VarisPage* page_;
	int type_;
	std::atomic<int> using_;
	std::mutex lock_;
	std::atomic<long> pos_;
public:
	PageWrapper();
	~PageWrapper();
	VarisPage* get(int pos);
	void release();
	void init(int fd,long pos,size_t pageSize);
	long getPos();
	bool initUsed();
};



#endif /* PAGEWRAPPER_H_ */
