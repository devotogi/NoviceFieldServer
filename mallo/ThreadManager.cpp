#include "pch.h"
#include "ThreadManager.h"
#include "IOCPCore.h"
ThreadManager* ThreadManager::_instance = nullptr;

void ThreadManager::Launch(unsigned int (*callback)(void*), void* args)
{
	Lock lock(&_cs);

	HANDLE hTread = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, callback, args, 0, &_threadId));
	_threads.push_back(hTread);
	_threadId++;

}

void ThreadManager::Join() 
{
	for (auto handle : _threads) 
	{
		WaitForSingleObject(handle, INFINITE);
	}
}