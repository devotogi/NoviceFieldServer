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
	i++ �� ��� �۵��ϴ��� 
	
	1. i���� �ӽ� ������ �����մϴ�.
	2. i���� ������ �ӽú����� + 1 �� �մϴ�.
	3. �ӽú����� ���� i ���� �����մϴ�.

	�ǻ� �ڵ�� ǥ�����ڸ�...
	int a = i;
	a = a + 1
	i = a;

*/

