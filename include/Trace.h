/*
 * Trace.h
 *
 *  Created on: Oct 23, 2010
 *      Author: jlalinde
 */

#ifndef __TRACE_H_
#define __TRACE_H_

#ifndef __STRING_
#include <string>
#define __STRING_
#endif

#ifdef __TRACE__

#ifndef __IOSTREAM_
#include <iostream>
#define __IOSTREAM_
#endif

using std::cerr;
using std::endl;

static inline void __TraceFn__(const string &msg, const string &file, const int line) {
	  cerr << getpid() << "-" << file << "(" << line << "):" << msg << endl;
};


#define Trace(x) __TraceFn__(x,__FILE__,__LINE__)

#else

#define Trace(x)

#endif

#endif /* __TRACE_H_ */
