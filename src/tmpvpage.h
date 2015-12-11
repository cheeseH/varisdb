/*
 * tmpvnode.h
 *
 *  Created on: 2015年12月11日
 *      Author: cheese
 */

#ifndef TMPVNODE_H_
#define TMPVNODE_H_

class TmpVPage{
public:
	VHead* head;
	VNode* vnws_;
	void dump();
	TmpVPage();
};



#endif /* TMPVNODE_H_ */
