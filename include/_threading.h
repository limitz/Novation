#ifndef LMTZ_THREAD
#define LMTZ_THREAD

#include <stdint.h>
#include <inttypes.h>
#include <pthread.h>
#include <exception.h>

GENERATE_EXCEPTION(ThreadException, Exception);

class Thread
{
	private:
	pthread_t _thread;
	bool _running;

	public:
	Thread()
	{
		_thread = 0;
		_running = false;
	}

	~Thread()
	{
		Kill();
	}

	void Start()
	{
		_running = true;
		pthread_create(&_thread, NULL, Thread::StartProc, this);
	}

	bool IsRunning() const
	{
		return _running;
	}

	void Stop()
	{
		_running = false;
	}

	void Join()
	{
		if (_thread) pthread_join(_thread, NULL);
		_thread = 0;
	}

	void Kill()
	{
		if (_thread) pthread_cancel(_thread);
		_thread = 0;
	}

	protected:
	virtual void OnStart() = 0;

	static void* StartProc(void *param)
	{
		Thread *t = (Thread*)param;
		t->OnStart();
		t->_running = false;
		t->_thread = 0;
	}
};

class Mutex
{
	private:
	pthread_mutex_t _mutex;
	uint32_t _depth;

	public: 
	Mutex()
	{
		pthread_mutexattr_t mta;
		pthread_mutexattr_init(&mta);
		pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(&_mutex, &mta);
	}

	~Mutex()
	{
		pthread_mutex_destroy(&_mutex);
	}

	void Lock(const char *description = 0)
	{
		//printf("LOCK   (%016lx) D[%d] %s\n", (uint64_t)(void*)this, _depth++, description ? description : "");
		pthread_mutex_lock(&_mutex);
	}

	void Unlock()
	{
		pthread_mutex_unlock(&_mutex);
		//printf("UNLOCK (%016lx) D[%d]\n", (uint64_t)(void*)this, --_depth);
	}
};

#endif
