#include "pch.h"
#include "Sector.h"
#include "GameSession.h"
#include "Player.h"
#include "BufferWriter.h"
#include "Monster.h"
#include "MapManager.h"
Sector::Sector()
{
	InitializeCriticalSection(&_cs);
}

Sector::~Sector()
{
	DeleteCriticalSection(&_cs);
}

void Sector::Set(Monster* monster)
{
	Lock lock(&_cs);

	_monsters.insert(monster);
}

void Sector::Set(GameSession* session)
{
	Lock lock(&_cs);

	_sessions.insert(session);
}

void Sector::Reset(GameSession* session)
{
	Lock lock(&_cs);

	_sessions.erase(session);
}

void Sector::Reset(Monster* monster)
{
	Lock lock(&_cs);

	_monsters.erase(monster);
}

void Sector::BroadCast(GameSession* session, BYTE* sendBuffer, int32 sendSize)
{
	Lock lock(&_cs);

	for (auto s : _sessions)
		s->Send(sendBuffer, sendSize);
}

void Sector::SendPlayerList(GameSession* session)
{
	Player* player = session->GetPlayer();

	BYTE sendBuffer[255];
	BufferWriter bw(sendBuffer);
	PacketHeader* pktHeader = bw.WriteReserve<PacketHeader>();

	int32 sessionId = session->GetSessionID();
	int8 playerState = (int8)player->GetState();
	int8 playerDir = (int8)player->GetDir();
	int8 playerMouseDir = (int8)player->GetMouseDir();
	Vector3 playerPos = player->GetPos();
	Quaternion playerQuaternion = player->GetCameraLocalRotation();
	Vector3 playerTarget = player->GetTarget();
	int8 playerMoveType= player->GetMoveType();
	float playerHp = player->GetHp();
	float playerMp = player->GetMp();
	int8 level = (int8) player->GetLevel();
	WCHAR* userName = player->GetPlayerName();
	int8 userNameSize = wcslen(userName) * sizeof(WCHAR);
	int8 playerType = player->GetPlayerType();

	bw.Write(sessionId);
	bw.Write(playerState);
	bw.Write(playerDir);
	bw.Write(playerMouseDir);
	bw.Write(playerPos);
	bw.Write(playerQuaternion);
	bw.Write(playerTarget);
	bw.Write(playerMoveType);
	bw.Write(playerHp);
	bw.Write(playerMp);
	bw.Write(level);
	bw.Write((int8)userNameSize);
	bw.WriteWString(userName, userNameSize);
	bw.Write(playerType);

	pktHeader->_type = PacketProtocol::S2C_PLAYERNEW;
	pktHeader->_pktSize = bw.GetWriterSize();

	Lock lock(&_cs);

	for (auto s : _sessions)
	{
		if (s->GetSessionID() == session->GetSessionID())
			continue;
		s->Send(sendBuffer, pktHeader->_pktSize);
	}

	int32 playerCount = _sessions.size();

	if (playerCount > 0)
	{
		int32 packetHeaderSize = 4;
		int32 playerCntSize = 4;
		int32 dataSize = 59;
		int32 playerCnt = _sessions.size();
		int32 va = 0;
		for (auto s : _sessions)
		{
			Player* p = s->GetPlayer();
			va += wcslen(p->GetPlayerName()) * sizeof(WCHAR);
		}

		const int32 allocSize = packetHeaderSize + playerCntSize + (playerCnt * dataSize) + va;
		BYTE* sendBuffer2 = new BYTE[allocSize];

		BufferWriter bw(sendBuffer2);
		PacketHeader* pktHeader = bw.WriteReserve<PacketHeader>();
		pktHeader->_type = PacketProtocol::S2C_PLAYERLIST;
		pktHeader->_pktSize = allocSize;

		bw.Write(playerCnt);
		for (auto s : _sessions)
		{
			Player* p = s->GetPlayer();
			bw.Write(s->GetSessionID());		//4
			bw.Write((int8)p->GetState());    // 1
			bw.Write((int8)p->GetDir());       // 1
			bw.Write((int8)p->GetMouseDir());   // 1
			bw.Write(p->GetPos()); // 19
			bw.Write(p->GetCameraLocalRotation()); //35
			bw.Write(p->GetTarget()); //47
			bw.Write((int8)p->GetMoveType()); // 48
			bw.Write(p->GetHp()); // 
			bw.Write(p->GetMp());// 72
			bw.Write((int8)p->GetLevel()); // 73
			int32 puserNameSize = wcslen(p->GetPlayerName()) * sizeof(WCHAR);
			bw.Write((int8)puserNameSize); // 74
			bw.WriteWString(p->GetPlayerName(), puserNameSize);
			bw.Write((int8)p->GetPlayerType()); // 75
		}

		session->Send(sendBuffer2, pktHeader->_pktSize);

		if (sendBuffer2)
		{
			delete[] sendBuffer2;
			sendBuffer2 = nullptr;
		}
	}
}

void Sector::SendMonsterList(GameSession* session)
{
	BYTE sendBuffer[4096] = {};
	BufferWriter bw(sendBuffer);
	PacketHeader* header = bw.WriteReserve<PacketHeader>();
	{
		Lock lock(&_cs);

		const int32 cnt = _monsters.size();

		bw.Write(cnt);

		for (auto monster : _monsters)
		{
			State monsterState = monster->GetState();

			if (monsterState == PATROL || monsterState == TRACE)
				monsterState = MOVE;

			bw.Write(monsterState);
			bw.Write(monster->GetMonsterType());
			bw.Write(monster->GetMonsterId());
			bw.Write(monster->GetPos());
			bw.Write(monster->GetHp());
			bw.Write(monster->GetVDir());
			bw.Write(monster->GetDest());

			int32 connerSize = monster->GetConner().size();
			bw.Write(connerSize);

			for (int32 i = 0; i < connerSize; i++)
			{
				Pos pos = monster->GetConner()[i];
				bw.Write(pos);
			}
		}
	}

	header->_pktSize = bw.GetWriterSize();
	header->_type = S2C_MONSTERRENEWLIST;

	session->Send(sendBuffer, bw.GetWriterSize());
}

void Sector::SendPlayerRemoveList(GameSession* session)
{
	Player* player = session->GetPlayer();

	BYTE sendBuffer[100];
	BufferWriter bw(sendBuffer);
	PacketHeader* pktHeader = bw.WriteReserve<PacketHeader>();

	int32 sessionId = session->GetSessionID();
	int8 playerState = (int8)player->GetState();
	int8 playerDir = (int8)player->GetDir();
	int8 playerMouseDir = (int8)player->GetMouseDir();
	Vector3 playerPos = player->GetPos();
	Quaternion playerQuaternion = player->GetCameraLocalRotation();

	bw.Write(sessionId);
	bw.Write(playerState);
	bw.Write(playerDir);
	bw.Write(playerMouseDir);
	bw.Write(playerPos);
	bw.Write(playerQuaternion);

	pktHeader->_type = PacketProtocol::S2C_PLAYEROUT;
	pktHeader->_pktSize = bw.GetWriterSize();

	Lock lock(&_cs);
	int32 playerCount = _sessions.size();

	if (playerCount > 0)
	{
		int32 packetHeaderSize = 4;
		int32 playerCntSize = 4;
		int32 dataSize = 4;
		int32 playerCnt = _sessions.size();

		const int32 allocSize = packetHeaderSize + playerCntSize + (playerCnt * dataSize);
		BYTE* sendBuffer2 = new BYTE[allocSize];

		BufferWriter bw(sendBuffer2);
		PacketHeader* pktHeader = bw.WriteReserve<PacketHeader>();
		pktHeader->_type = PacketProtocol::S2C_PLAYERREMOVELIST;
		pktHeader->_pktSize = packetHeaderSize + playerCntSize + (playerCnt * dataSize);

		bw.Write(playerCnt);
		for (auto s : _sessions)
		{
			Player* p = s->GetPlayer();
			bw.Write(s->GetSessionID());
		}

		session->Send(sendBuffer2, pktHeader->_pktSize);

		if (sendBuffer2)
		{
			delete[] sendBuffer2;
			sendBuffer2 = nullptr;
		}

	}

	for (auto s : _sessions)
	{
		if (s->GetSessionID() == session->GetSessionID())
			continue;

		s->Send(sendBuffer, pktHeader->_pktSize);
	}

}

void Sector::SendMonsterRemoveList(GameSession* session)
{
	BYTE sendBuffer[4096] = {};
	BufferWriter bw(sendBuffer);
	PacketHeader* header = bw.WriteReserve<PacketHeader>();
	{
		Lock lock(&_cs);

		const int32 cnt = _monsters.size();

		bw.Write(cnt);

		for (auto monster : _monsters)
		{
			bw.Write(monster->GetMonsterId());
		}
	}

	header->_pktSize = bw.GetWriterSize();
	header->_type = S2C_MONSTERREMOVELIST;

	session->Send(sendBuffer, bw.GetWriterSize());
}
