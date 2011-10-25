#ifndef __SHAREDMEMORY_H_
#define __SHAREDMEMORY_H_

#ifndef __STRING__
#include <string>
#define __STRING__
#endif

#ifndef __SHAREDMEMORYEXCEPTION_H_
#include <SharedMemoryException.h>
#define __SHAREDMEMORYEXCEPTION_H_
#endif

#ifndef __ERRNO_H_
#include <errno.h>
#define __ERRNO_H_
#endif

#ifndef __SYS_TYPES_H_
#include <sys/types.h>
#define __SYS_TYPES_H_
#endif

#ifndef __SYS_MMAN_H_
#include <sys/mman.h>
#define __SYS_MMAN_H_
#endif

#ifndef __FCNTL_H_
#include <fcntl.h>
#define __FCNTL_H_
#endif

#ifndef __CSTRING_
#include <cstring>
#define __CSTRING_
#endif

#ifndef __CSTDIO_
#include <cstdio>
#define __CSTDIO_
#endif

#ifndef __TRACE_H
#include <Trace.h>
#define __TRACE_H
#endif

#ifndef __TOSTRING_H
#include <ToString.h>
#define __TOSTRING_H
#endif

using namespace std;

template<class T>
class SharedMemory {
	int *instances; // count how many users have instantiated the memory
	string instanceName;
	int instanceDescriptor;

	T *mem; // shared memory
	string name;
	int descriptor;

	sem_t *semaphore; // semaphore to avoid problems counting users
	string semName;

public:
	SharedMemory(const string &n) throw(SharedMemoryException);

	~SharedMemory();

	T &operator()() {
		return *mem;
	};
};

template<class T>
SharedMemory<T>::SharedMemory(const string &n) throw(SharedMemoryException) {
	name = n;
	Trace("SharedMemory constructor");
	// The strategy is simple: Try to create the shared memory segment. If it
	// does exist, then open it. The strategy is enclosed in a do cicle
	// because there is a change that in the interval between the first open and
	// the second one the shared memory segment is deleted.
	do {
		descriptor = shm_open(name.c_str(), O_RDWR | O_CREAT | O_EXCL, S_IRWXU);
		if (descriptor != -1) {
			Trace("Shared memory segment " + name + " created");
			if (ftruncate(descriptor, sizeof(T)) == -1) {
				int errorNumber = errno;
				Trace("ftruncate: " + name + ":" + strerror(errorNumber));
				close(descriptor);
				shm_unlink(name.c_str());
				throw new SharedMemoryException(name + ":" + strerror(errorNumber));
			} else {
				// creates the semaphore BEFORE the shared memory
				// keep in mind that only the process that creates the shared memory segment executes this code
				semName = name + ".semaphore";
				sem_unlink(semName.c_str());
				semaphore = sem_open(semName.c_str(), O_CREAT | O_EXCL, S_IRWXU, 1);
				if (semaphore == SEM_FAILED) {
					int errorNumber = errno;
					Trace("sem_open: " + instanceName + ":" + strerror(errorNumber));
					close(descriptor);
					shm_unlink(name.c_str());
					throw new SharedMemoryException(instanceName + ":" + strerror(errorNumber));
				}
				Trace("Semaphore " + semName + " created");
				instanceName = name + ".descriptor";
				instanceDescriptor = shm_open(instanceName.c_str(), O_RDWR | O_CREAT | O_EXCL, S_IRWXU);
				if (instanceDescriptor == -1) {
					int errorNumber = errno;
					Trace("shm_open: " + instanceName + ":" + strerror(errorNumber));
					close(descriptor);
					shm_unlink(name.c_str());
					throw new SharedMemoryException(instanceName + ":" + strerror(errorNumber));
				}
				Trace("Shared memory segment" + instanceName + " created");
				if (ftruncate(instanceDescriptor, sizeof(int)) == -1) {
					int errorNumber = errno;
					Trace("ftruncate: " + name + ":" + strerror(errorNumber));
					close(descriptor);
					shm_unlink(name.c_str());
					close(instanceDescriptor);
					shm_unlink(instanceName.c_str());
					throw new SharedMemoryException(name + ":" + strerror(errorNumber));
				}
				instances = (int *) mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, instanceDescriptor, 0);
				// *instances = 0; is not needed because ftruncated sets the mem to 0
			}
		} else { //	 descriptor == -1
			if (errno == EEXIST) { // SharedMemory already exists
				descriptor = shm_open(name.c_str(), O_RDWR, S_IRWXU);
				if (descriptor == -1 && errno != ENOENT) {
					Trace("shm_open: " + name + ":" + strerror(errno));
					throw new SharedMemoryException(name + ":" + strerror(errno));
				} else if (descriptor != -1) {
					Trace("Shared memory segment " + name + " open");
					instanceName = name + ".descriptor";
					do {
						instanceDescriptor = shm_open(instanceName.c_str(), O_RDWR, S_IRWXU);
					} while (instanceDescriptor == -1 && errno == ENOENT);
					Trace("Shared memory segment" + instanceName + " open");
					if (instanceDescriptor == -1) {
						int errorNumber = errno;
						Trace("shm_open: " + instanceName + ":" + strerror(errorNumber));
						close(descriptor);
						shm_unlink(name.c_str());
						throw new SharedMemoryException(instanceName + ":" + strerror(errorNumber));
					}
					instances = (int *) mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, instanceDescriptor, 0);
					semName = name + ".semaphore";
					semaphore = sem_open(semName.c_str(), O_RDWR, S_IRWXU, 1);
					if (semaphore == SEM_FAILED) {
						int errorNumber = errno;
						Trace("sem_open: " + instanceName + ":" + strerror(errorNumber));
						close(instanceDescriptor);
						shm_unlink(instanceName.c_str());
						close(descriptor);
						shm_unlink(name.c_str());
						throw new SharedMemoryException(instanceName + ":" + strerror(errorNumber));
					}
					Trace("Semaphore " + semName + " open");
				}
			} else {
					Trace("shm_open: " + name + ":" + strerror(errno));
					throw new SharedMemoryException(name + ":" + strerror(errno));
			}
		}
	} while (descriptor == -1);
	mem = (T *) mmap(NULL, sizeof(T), PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);
	Trace("Waiting on semaphore " + semName);
	while (sem_wait(semaphore) == EINTR)
		;
	(*instances)++;
	Trace("Number of instances " + ToString(*instances));
	sem_post(semaphore);
	Trace("Posting on semaphore " + semName);
};


template<class T>
SharedMemory<T>::~SharedMemory() {
	Trace("Shared memory destructor");
	close(descriptor);
	Trace("Waiting on semaphore " + semName);
	while (sem_wait(semaphore) == EINTR)
		;
	(*instances)--;
	Trace("Number of instances " + ToString(*instances));
	if (*instances == 0) {
		sem_post(semaphore);
		Trace("Post on semaphore " + semName);
		shm_unlink(name.c_str());
		shm_unlink(instanceName.c_str());
		sem_unlink(semName.c_str());
		Trace("Shared memory segments " + name + " and " + instanceName + " are unlinked");
		Trace("Sempahore " + semName + " is unlinked");
	} else {
		sem_post(semaphore);
		Trace("Post on semaphore " + semName);
	}
}


#endif /*__SHAREDMEMORY_H_*/
