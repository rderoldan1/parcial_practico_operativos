#ifndef __SHAREDMEMORYEXCEPTION_H_
#define __SHAREDMEMORYEXCEPTION_H_

#ifndef __EXCEPTION__
#include <exception>
#define __EXCEPTION__
#endif

#ifndef __STRING__
#include <string>
#define __STRING__
#endif

using namespace std;

class SharedMemoryException : public exception {

	string message;
public:
	SharedMemoryException(const string &m) throw() {
		message = m;
	}

	virtual ~SharedMemoryException() throw() {};
	
	const char *what() const throw() {
		return message.c_str();
	}
	
};

#endif /*__SHAREDMEMORYEXCEPTION_H_*/
