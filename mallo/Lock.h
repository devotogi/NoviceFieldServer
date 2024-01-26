#pragma once
class LockObject
{
public:
    LockObject() {};
    ~LockObject() {};

    virtual void Lock() abstract;
    virtual void UnLock() abstract;
};

class CriticalSectionObject : public LockObject
{
private:
    CRITICAL_SECTION _cs;

public:
    CriticalSectionObject() : LockObject()
    {
        InitializeCriticalSection(&_cs);
    }

    ~CriticalSectionObject()
    {
        DeleteCriticalSection(&_cs);
    }

public:
    virtual void Lock() override
    {
        EnterCriticalSection(&_cs);
    }

    virtual void UnLock() override
    {
        LeaveCriticalSection(&_cs);
    }
};

class MutexObject : public LockObject
{
private:
    HANDLE _mutex;

public:
    MutexObject() : LockObject()
    {
        _mutex = CreateMutex(
            NULL,
            FALSE,
            NULL);
    }

    ~MutexObject()
    {
        CloseHandle(_mutex);
    }

public:
    virtual void Lock() override
    {
        WaitForSingleObject(_mutex, INFINITE);
    }

    virtual void UnLock() override
    {
        ReleaseMutex(_mutex);
    }
};

class LockGuard
{
private:
    LockObject* _obj;

public:
    LockGuard(LockObject* obj) : _obj(obj)
    {
        _obj->Lock();
    }

    ~LockGuard()
    {
        _obj->UnLock();
    }
};

