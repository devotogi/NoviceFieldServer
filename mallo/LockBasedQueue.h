#pragma once
template<typename T>
class LockBasedQueue
{
private:
	CRITICAL_SECTION			_cs;
	std::queue<T>				_queue;

public:
	LockBasedQueue()
	{
		InitializeCriticalSection(&_cs);
	}

	~LockBasedQueue()
	{
		DeleteCriticalSection(&_cs);
	}

	bool Empty() 
	{
		EnterCriticalSection(&_cs);

		bool ret = _queue.empty();

		LeaveCriticalSection(&_cs);
		
		return ret;
	}


	void Push(const T& val) 
	{
		EnterCriticalSection(&_cs);

		_queue.push(val);

		LeaveCriticalSection(&_cs);
	}

	T Pop()
	{
		EnterCriticalSection(&_cs);

		T ret = _queue.front();
		_queue.pop();

		LeaveCriticalSection(&_cs);
		
		return ret;
	}
};

