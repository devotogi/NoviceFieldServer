#pragma once
template<typename T>
class LockBasedQueue
{
private:
	CriticalSectionObject		_cs;
	std::queue<T>				_queue;

public:
	LockBasedQueue() 
	{
		// InterlockedIncrement()
	}
	~LockBasedQueue(){}

	void Push(const T& val) 
	{
		LockGuard lock(&_cs);
		_queue.push(val);
	}

	T Pop()
	{
		LockGuard lock(&_cs);
		T ret = nullptr;
		if (_queue.empty() == false)
		{
			ret = _queue.front();
			_queue.pop();
		}
		return ret;
	}
};

/*
	i++ 가 어떻게 작동하는지 
	
	1. i값을 임시 변수에 복사합니다.
	2. i값을 복사한 임시변수에 + 1 을 합니다.
	3. 임시변수의 값을 i 값에 대입합니다.

	의사 코드로 표현하자면...
	int a = i;
	a = a + 1
	i = a;

*/

