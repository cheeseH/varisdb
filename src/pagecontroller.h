/*
 * pagecontroller.h
 *
 *  Created on: 2015年12月1日
 *      Author: cheese
 */

#ifndef PAGECONTROLLER_H_
#define PAGECONTROLLER_H_

#include <map>
#include <list>
class PageController{
public:
	PageController();
	~PageController();
	PageWrapper* get(long pos);

private:
	friend class LruList;
	void remove(long pos);
	std::map<long,PageWrapper*> pws_;
	std::mutex maplock_;
	std::list<PageWrapper*> freelist_;
	int fd_;
	int pageSize_;

};



#endif /* PAGECONTROLLER_H_ */
