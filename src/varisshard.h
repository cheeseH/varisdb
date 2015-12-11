/*
 * varisshared.h
 *
 *  Created on: 2015年11月25日
 *      Author: cheese
 */

#ifndef VARISSHARD_H_
#define VARISSHARD_H_
class VarisLRUList;

struct ShardHead{
	size_t level;
	size_t len;
	long root;

};
class VarisShard{
private:
	VarisDB* parent_;
	int id_;
	VarisLRUList* lrulist_;
	size_t level_;
	VarisPage* root_;
	int idxfile_;
	int contentflie_;
	int freefile_;
public:
	VarisShard();
	void init(VarisDB* parent,int id_);
	~VarisShard();
	const char* get(const char* key,long hash);
	bool push(const char* key,const char* value);
	bool set(const char* key,const char* value);
	bool update(const char* key,const char* value);
	const char*  remove(const char* key,long hash);
	long insertToHeap(const char* key,const char* value);
private:
	const char* getDataInContent(long dataPos,const char* key);

};



#endif /* VARISSHARD_H_ */
