#ifndef __SIMPLESEMAPHOREEXCEPTION_H_
#define __SIMPLESEMAPHOREEXCEPTION_H_

#ifndef __EXCEPTION__
#include <exception>
#define __EXCEPTION__
#endif

#ifndef __STRING__
#include <string>
#define __STRING__
#endif

using namespace std;

class SimpleSemaphoreException : public exception {

	string message;
public:
	SimpleSemaphoreException(const string &m) throw() {
		message = m;
	}

	virtual ~SimpleSemaphoreException() throw() {};
	
	const char *what() const throw() {
		return message.c_str();
	}
	
};

#endif /*__SIMPLESEMAPHOREEXCEPTION_H_*/
