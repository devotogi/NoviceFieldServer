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
		Lock lock(&_cs);

		bool ret = _queue.empty();

		return ret;
	}


	void Push(const T& val) 
	{
		Lock lock(&_cs);

		_queue.push(val);
	}

	T Pop()
	{
		Lock lock(&_cs);

		T ret = _queue.front();
		_queue.pop();
		
		return ret;
	}
};

