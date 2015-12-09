/*
 * functions.h
 *
 *  Created on: 2015年11月26日
 *      Author: cheese
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_



template<typename T>
class __default_cmp_less {
public:
	bool operator () (T& v1, T& v2) {
		return v1 < v2;
	}
};

template<typename T>
class __default_cmp_equ {
public:
	bool operator () (T& v1, T& v2) {
		return v1 == v2;
	}
};


template<typename IT, typename T, typename CMP = __default_cmp_less<T>, typename EQU = __default_cmp_equ<T> >
IT binarySearch(IT begin, IT end, T& target, bool& isequal, CMP cmp = __default_cmp_less<T>(), EQU equ = __default_cmp_equ<T>()) {
	isequal = false;
	while(true) {
		if(begin == end) {
			return begin;
		}
		IT mdl = begin + (end - begin) / 2;
		if(equ(target, *mdl)) {
			isequal = true;
			return mdl;
		}
		if(cmp(target, *mdl)) {
			end = mdl;
		} else {
			begin = mdl + 1;
		}
	}
}




#endif /* FUNCTIONS_H_ */
