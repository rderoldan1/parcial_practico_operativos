/*
 * ToString.h
 *
 *  Created on: Oct 23, 2010
 *      Author: jlalinde
 */

#ifndef __TOSTRING_H_
#define __TOSTRING_H_

#ifndef __STRING_
#include <string>
#define __STRING_
#endif

#ifndef __SSTREAM_
#include <sstream>
#define __SSTREAM_
#endif

using std::string;
using std::stringstream;

template<class T>
const string ToString(const T &val) {

	stringstream out;
	out << val;
	return out.str();
}

#endif /* TOSTRING_H_ */
