#ifndef LOCK_H
#define LOCK_H
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

//xinhaoliang
class Sem
{
private:
	sem_t sem;
public:
	Sem();
	~Sem();
	bool wait();
	bool post();
};

Sem::Sem()
{
	if (sem_init(&sem, 0, 0) != 0)//信号量的初始值和和基于内存的信号量
		std::cerr << "sem init error." << std::endl;
}

Sem::~Sem()
{
	sem_destroy(&sem);
}

bool Sem::wait()
{
	return sem_wait(&sem) == 0 ? true : false;
}

bool Sem::post()
{
	return sem_post(&sem) == 0 ? true : false;
}


class Mutex
{
private:
	pthread_mutex_t mutex;

public:
	Mutex();
	~Mutex();
	bool mutex_lock();
	bool mutex_unlock();
};

Mutex::Mutex()
{
	if (pthread_mutex_init(&mutex, NULL) != 0)//可用PTHRAD_MUTEX_INITIALIZER宏初始化
		std::cerr << "mutex init error" << std::endl;
}

Mutex::~Mutex()
{
	pthread_mutex_destroy(&mutex);
}

bool Mutex::mutex_lock()
{
	return pthread_mutex_lock(&mutex) == 0 ? true : false;
}

bool Mutex::mutex_unlock()
{
	return pthread_mutex_unlock(&mutex) == 0 ? true : false;
}


class Cond
{
private:
	pthread_mutex_t mutex;
	pthread_cond_t cond;

public:
	Cond();
	~Cond();
	void wait();
	bool signal();
	bool broadcast();
};

Cond::Cond()
{
	if (pthread_mutex_init(&mutex, NULL) != 0)
	{
		std::cerr << "Cond mutex init error" << std::endl;
		exit(0);
	}
	if (pthread_cond_init(&cond, NULL) != 0)
	{
		std::cerr << "Cond cond init error" << std::endl;
		pthread_mutex_destroy(&mutex);
		exit(0);
	}
}

Cond::~Cond()
{
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
}

void Cond::wait()
{
	// int rs = 0;
	pthread_mutex_lock(&mutex);
	pthread_cond_wait(&cond, &mutex);
	pthread_mutex_unlock(&mutex);
	// return rs == 0 ? true : false;
}

bool Cond::signal()
{
	return pthread_cond_signal(&cond) == 0 ? true : false;
}

bool Cond::broadcast()
{
	return pthread_cond_broadcast(&cond);
}

#endif
