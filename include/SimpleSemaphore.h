#ifndef __SIMPLESEMAPHORE_H_
#define __SIMPLESEMAPHORE_H_

#ifndef __STRING_
#include <string>
#define __STRING_
#endif

#ifndef __SEMAPHORE_H_
#include <semaphore.h>
#define __SEMAPHORE_H_
#endif

#ifndef __SIMPLESEMAPHOREEXCEPTION_H_
#include <SimpleSemaphoreException.h>
#define __SIMPLESEMAPHOREEXCEPTION_H_
#endif

#ifndef __FCNTL_H_
#include <fcntl.h>
#define __FCNTL_H_
#endif

#ifndef __SYS_MMAN_H_
#include <sys/mman.h>
#define __SYS_MMAN_H_
#endif

#ifndef __ERRNO_H_
#include <errno.h>
#define __ERRNO_H_
#endif

#ifndef __CSTDIO_
#include <cstdio>
#define __CSTDIO_
#endif

#ifndef __CSTRING_
#include <cstring>
#define __CSTRING_
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

class SimpleSemaphore {
	int *instances; // count how many users have instantiated the memory
	string instanceName;
	int instanceDescriptor;

	sem_t *sem;
	string sname;

	sem_t *semaphore; // semaphore to avoid problems counting users
	string semName;

  public:
	SimpleSemaphore(const string &name, int value = 1) throw (SimpleSemaphoreException);
	virtual ~SimpleSemaphore();

	void Wait() {while (sem_wait(sem) == EINTR) ;};
	void Signal() {sem_post(sem);};
	int Value() const {
		int value = sem_getvalue(sem, &value);
		return value;
	};
};

inline SimpleSemaphore::SimpleSemaphore(const string &name, int value) throw (SimpleSemaphoreException) {

	Trace("SimpleSemaphore constructor");
	sname = name;
	do {
		sem = sem_open(sname.c_str(), O_RDWR | O_CREAT | O_EXCL, S_IRWXU, value);
		if (sem != SEM_FAILED) {
			Trace("Semaphore " + sname + " created");
			// creates the semaphore BEFORE the shared memory
			// keep in mind that only the process that creates the shared memory segment executes this code
			semName = sname + ".semaphore";
			sem_unlink(semName.c_str());
			semaphore = sem_open(semName.c_str(), O_CREAT | O_EXCL, S_IRWXU, 1);
			if (semaphore == SEM_FAILED) {
				int errorNumber = errno;
				Trace("sem_open: " + instanceName + ":" + strerror(errorNumber));
				sem_close(sem);
				sem_unlink(sname.c_str());
				throw new SimpleSemaphoreException(instanceName + ":" + strerror(errorNumber));
			}
			Trace("Semaphore " + semName + " created");
			instanceName = name + ".descriptor";
			instanceDescriptor = shm_open(instanceName.c_str(), O_RDWR | O_CREAT | O_EXCL, S_IRWXU);
			if (instanceDescriptor == -1) {
				int errorNumber = errno;
				Trace("shm_open: " + instanceName + ":" + strerror(errorNumber));
				sem_close(sem);
				sem_unlink(sname.c_str());
				throw new SimpleSemaphoreException(instanceName + ":" + strerror(errorNumber));
			}
			Trace("Shared memory segment " + instanceName + " created");
			if (ftruncate(instanceDescriptor, sizeof(int)) == -1) {
				int errorNumber = errno;
				Trace("ftruncate: " + name + ":" + strerror(errorNumber));
				sem_close(sem);
				sem_unlink(sname.c_str());
				close(instanceDescriptor);
				shm_unlink(instanceName.c_str());
				throw new SimpleSemaphoreException(name + ":" + strerror(errorNumber));
			}
			instances = (int *) mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, instanceDescriptor, 0);
			// *instances = 0; is not needed because ftruncate sets the mem to 0
		} else { //	 descriptor == -1
			if (errno == EEXIST) { // Semaphore already exists
				sem = sem_open(sname.c_str(), O_RDWR, S_IRWXU);
				if (sem == SEM_FAILED && errno != ENOENT) {
					Trace("sem_open: " + sname + ":" + strerror(errno));
					throw new SimpleSemaphoreException(sname + ":" + strerror(errno));
				} else if (sem != SEM_FAILED) {
					Trace("Semaphore " + sname + " opened");
					instanceName = sname + ".descriptor";
					do {
						instanceDescriptor = shm_open(instanceName.c_str(), O_RDWR, S_IRWXU);
					} while (instanceDescriptor == -1 && errno == ENOENT);
					Trace("Shared memory segment " + instanceName + " opened");
					if (instanceDescriptor == -1) {
						int errorNumber = errno;
						Trace("shm_open: " + instanceName + ":" + strerror(errorNumber));
						sem_close(sem);
						shm_unlink(sname.c_str());
						throw new SimpleSemaphoreException(instanceName + ":" + strerror(errorNumber));
					}
					instances = (int *) mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, instanceDescriptor, 0);
					semName = name + ".semaphore";
					semaphore = sem_open(semName.c_str(), O_RDWR, S_IRWXU, 1);
					if (semaphore == SEM_FAILED) {
						int errorNumber = errno;
						Trace("sem_open: " + instanceName + ":" + strerror(errorNumber));
						close(instanceDescriptor);
						shm_unlink(instanceName.c_str());
						sem_close(sem);
						shm_unlink(name.c_str());
						throw new SimpleSemaphoreException(instanceName + ":" + strerror(errorNumber));
					}
					Trace("Semaphore " + semName + " opened");
				}
			} else {
					Trace("sem_open: " + sname + ":" + strerror(errno));
					throw new SimpleSemaphoreException(sname + ":" + strerror(errno));
			}
		}
	} while (sem == SEM_FAILED);
	Trace("Waiting on " + semName);
	while (sem_wait(semaphore) == EINTR)
		;
	(*instances)++;
	Trace("Number of instances of " + sname + ": " + ToString(*instances));
	sem_post(semaphore);
	Trace("Post on " + sname);
}

inline SimpleSemaphore::~SimpleSemaphore() {

	Trace("SimpleSemaphore destructor");
	sem_close(sem);
	Trace("Waiting on " + semName);
	while (sem_wait(semaphore) == EINTR)
		;
	(*instances)--;
	Trace("Number of instances of " + sname + ": " + ToString(*instances));
	if (*instances == 0) {
		sem_post(semaphore);
		Trace("Post on " + semName);
		sem_unlink(sname.c_str());
		shm_unlink(instanceName.c_str());
		sem_unlink(semName.c_str());
		Trace("Semaphores " + sname + " and " + semName + " are unlinked");
		Trace("Shared memory segment " + instanceName + " is unlinked");
	} else {
		sem_post(semaphore);
		Trace("Post on " + semName);
	}
}

#endif /* __SIMPLESEMAPHORE_H_ */
