/*
 * varisdb.h
 *
 *  Created on: 2015年11月25日
 *      Author: cheese
 */

#ifndef VARISDB_H_
#define VARISDB_H_

class VarisConfig;
class VarisDB{
private:
public:
	static VarisDB* open(const char* dbname);
	static VarisDB* open(const char* dbname,const VarisConfig& config);
	VarisDB();
	~VarisDB();
	const char* get(const char* key);
	bool push(const char* key,const char* value);
	bool set(const char* key,const char* value);
	bool update(const char* key,const char* value);
	const char*  remove(const char* key);

};



#endif /* VARISDB_H_ */
