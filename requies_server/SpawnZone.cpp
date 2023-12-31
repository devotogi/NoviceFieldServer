#include "pch.h"
#include "SpawnZone.h"
#include "MonsterManager.h"
#include "Monster.h"
#include "MapManager.h"
SpawnZone::SpawnZone(int32 index, int32 maxSpawnCount, const BoundBox& boundBox, MonsterType monsterType) : _index(index), _maxSpawnCnt(maxSpawnCount), _boundBox(boundBox), _monsterType(monsterType), _nowSpawnCnt(0)
{
	InitializeCriticalSection(&_cs);
}

SpawnZone::~SpawnZone()
{
	DeleteCriticalSection(&_cs);
}

void SpawnZone::Update(int32 currentTick)
{
	int32 deltaTick = currentTick - _lastTick;
	_sumTick += deltaTick;
	_lastTick = currentTick;

	if (_sumTick < SPAWN_TICK) return;

	uint32 spawnCnt = _maxSpawnCnt - _nowSpawnCnt;

	for (int i = 0; i < spawnCnt; i++)
		Spawn();

	_sumTick = 0;
}

void SpawnZone::Spawn()
{
	Monster* monster = MonsterManager::GetInstnace()->PopMonster(_monsterType);
	
	if (monster == nullptr) return;

	Vector3 spawnPos = RandomPos();
	int32 spawnIdnex = _index;

	monster->Spawn(spawnPos, spawnPos, spawnIdnex);
	MapManager::GetInstance()->Set(monster);
	IncreaseSpawnCnt();
}

void SpawnZone::IncreaseSpawnCnt()
{
	_nowSpawnCnt++;
}

void SpawnZone::DecreaseSpawnCnt()
{
	_nowSpawnCnt--;
}

Vector3 SpawnZone::RandomPos()
{
	thread_local std::mt19937 generator(std::random_device{}());
	bool canSpawn = false;
	int32 x = 0;
	int32 z = 0;

	while (!canSpawn)
	{
		std::uniform_int_distribution<int> xPos(_boundBox.minX, _boundBox.maxX-1);
		std::uniform_int_distribution<int> zPos(_boundBox.minZ, _boundBox.maxZ-1);

		x = xPos(generator);
		z = zPos(generator);

		canSpawn = MapManager::GetInstance()->CanGo(z, x);
	}

	Vector3 v3 = { static_cast<float>(x) + 0.5f, 0.f, static_cast<float>(z) + 0.5f };

	return v3;
}
