#pragma once
#include "Monster.h"
class GameSession;
class Sector
{
private:
	CriticalSectionObject		_cs;
	std::set<GameSession*> _sessions;
	std::set<Monster*>	   _monsters;
public:
	Sector();
	~Sector();

	void Set(Monster* monster);
	void Reset(Monster* monster);
	void Set(GameSession* session);
	void Reset(GameSession* session);
	void BroadCast(GameSession* session, BYTE* sendBuffer, int32 sendSize);
	void SendPlayerList(GameSession* session);
	void SendMonsterList(GameSession* session);
	void SendPlayerRemoveList(GameSession* session);
	void SendMonsterRemoveList(GameSession* session);
	std::set<GameSession*>& GetSessions() { return _sessions; }
	std::set<GameSession*> GetSessionCopy() { return _sessions; }
};

