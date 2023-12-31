#include "pch.h"
#include "MonsterManager.h"
#include "Monster.h"
#include "BufferWriter.h"
#include "MapManager.h"
#include "MonsterTable.h"
#include "Player.h"
#include "GameSession.h"
MonsterManager::MonsterManager()
{
	InitializeCriticalSection(&_cs);
}

MonsterManager::~MonsterManager()
{
	DeleteCriticalSection(&_cs);
}

void MonsterManager::Init(int32 totalCnt)
{
	_monsterTotalCnt = totalCnt;
	_monsterTable.resize(_monsterTotalCnt + 1);

	int32 monsterId = 0;

	for (int32 i = 0; i < totalCnt; i++)
	{
		_monsterTable[monsterId] = Monster(monsterId);
		_waitSpawnQueue.push(&_monsterTable[monsterId]);
		monsterId++;
	}
}

void MonsterManager::PushMonster(Monster* monster)
{
	MapManager::GetInstance()->ReSet(monster);
	monster->Clear();
	MapManager::GetInstance()->ReSpawn(monster);
	_waitSpawnQueue.push(monster);
}

Monster* MonsterManager::PopMonster(MonsterType type)
{
	Monster* ret = nullptr;

	if (_waitSpawnQueue.empty()) return ret;

	ret = _waitSpawnQueue.front();
	_waitSpawnQueue.pop();

	ret->SetMonsterType(type);
	MonsterTable::GetInstnace()->SetMonsterAttribute(ret);

	return ret;
}

void MonsterManager::Update(int32 currentTick)
{
	_lastTick =_lastTick == 0 ? currentTick : _lastTick;
	int32 deltaTick = currentTick - _lastTick;

	_sumTick += deltaTick;
	_lastTick = currentTick;

	for (int32 i = 0; i < _monsterTotalCnt; i++) 
	{
		if (_monsterTable[i].GetState() == State::RESPAWN)
			continue;

		_monsterTable[i].Update(deltaTick);
	}
}

void MonsterManager::AttackedMonster(int32 monsterId, Creature* attacker, int32 damage)
{
	Vector3 attackerPos = attacker->GetPos();
	float playerDamage = static_cast<Player*>(attacker)->GetDamage();

	if (_monsterTable[monsterId].IsDead()) return;

	Vector3 playerPos = attackerPos;
	Vector3 s_monsterPos = _monsterTable[monsterId].GetPos();

	playerPos.y = 0;
	s_monsterPos.y = 0;
	
	Vector3 dist = playerPos - s_monsterPos;
	float distF = dist.Magnitude();

	if (distF >= 20) return;

	_monsterTable[monsterId].Attacked(attacker, playerDamage);

	BYTE sendBuffer[100];
	BufferWriter bw(sendBuffer);
	PacketHeader* pktHeader = bw.WriteReserve<PacketHeader>();

	 int32 mtType = _monsterTable[monsterId].GetMonsterType();
	float mtHp = _monsterTable[monsterId].GetHp();

	bw.Write(monsterId);
	bw.Write(mtType);
	bw.Write(mtHp);

	pktHeader->_type = PacketProtocol::S2C_MONSTERINFO;
	pktHeader->_pktSize = bw.GetWriterSize();
	static_cast<Player*>(attacker)->GetSession()->Send(sendBuffer,bw.GetWriterSize());
}
