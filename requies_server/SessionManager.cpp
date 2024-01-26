#include "pch.h"
#include "SessionManager.h"
#include "GameSession.h"
#include "BufferWriter.h"
#include "Player.h"

SessionManager* SessionManager::_instance = nullptr;

void SessionManager::AddSession(int32 sessionId, GameSession* session)
{
	LockGuard lock(&_cs);

	_sessions.insert({ sessionId,session });
}

void SessionManager::PopSession(int32 sessionId)
{
	LockGuard lock(&_cs);

	_sessions.erase(sessionId);
}

void SessionManager::BroadCast(BYTE* dataPtr, int32 dataSize)
{
	LockGuard lock(&_cs);

	for (auto& session : _sessions)
		session.second->Send(dataPtr, dataSize);
}

void SessionManager::GetSessionId(int32& sessionId)
{
	LockGuard lock(&_cs);

	_sessionId++;
	sessionId = _sessionId;
}

GameSession* SessionManager::GetSession(int32 sessionId)
{
	GameSession* ret = nullptr;

	{
		LockGuard lock(&_cs);
		auto it = _sessions.find(sessionId);

		if (it != _sessions.end())
			ret = it->second;
	}
	return ret;
}
