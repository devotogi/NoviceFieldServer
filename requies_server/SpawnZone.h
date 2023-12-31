#pragma once
#include "FPS.h"

class SpawnZone : public FPS
{
	enum
	{
		SPAWN_TICK = 10000,
	};

private:
	BoundBox			_boundBox;
	uint32				_index;
	uint32				_nowSpawnCnt;
	uint32				_maxSpawnCnt;
	MonsterType			_monsterType;
	CRITICAL_SECTION	_cs;

public:
	SpawnZone(int32 index, int32 maxSpawnCount, const BoundBox& boundBox, MonsterType monsterType);
	~SpawnZone();

	void Update(int32 currentTick);
	void Spawn();
	void IncreaseSpawnCnt();
	void DecreaseSpawnCnt();

private:
	Vector3 RandomPos();
};

