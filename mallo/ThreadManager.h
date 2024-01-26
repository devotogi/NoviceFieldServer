#pragma once
class ThreadManager
{
private:
	static ThreadManager*	_instance;
	uint32					_threadId = 0;
	std::vector<HANDLE>		_threads;
	CriticalSectionObject	_cs;

public:
	ThreadManager()
	{
	}

	~ThreadManager()
	{
	}

	void Launch(unsigned int (*callback)(void*), void* args);
	void Join();

	static ThreadManager* GetInstance()
	{
		static ThreadManager instance;
		return &instance;
	}
};