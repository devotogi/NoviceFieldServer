#include "pch.h"
#include "MapManager.h"
#include "Sector.h"
#include "SpawnZone.h"
#include "Player.h"
#include "GameSession.h"
#include "BufferWriter.h"
#define SectorBlockSize 32
#define ADJACENT_COUNT 9

void MapManager::MapLoad(const char* filePath)
{
	FILE* fp = NULL;
	fopen_s(&fp, filePath, "r");

	fseek(fp, 0, SEEK_END);
	const int32 size = ftell(fp);

	char* buffer = new char[size];
	::memset(buffer, 0, size);

	fseek(fp, 0, SEEK_SET);
	fread(buffer, size, 1, fp);
	fclose(fp);

	char* dataPtr = (char*)buffer;
	const int32 fileSize = (*(int32*)dataPtr);		dataPtr += 4;
	_zSize = (*(int32*)dataPtr);					dataPtr += 4;
	_xSize = (*(int32*)dataPtr);					dataPtr += 4;

	_closeSearch.resize(_zSize + 1, std::vector<int32>(_xSize + 1, 0));
	_objectSearch.resize(_zSize + 1, std::vector<std::set<GameObject*>>(_xSize + 1));

	for (int32 z = 0; z <= _zSize; z++)
	{
		for (int32 x = 0; x <= _xSize; x++)
		{
			int32 data = (*(int32*)dataPtr); dataPtr += 4;
			_closeSearch[z][x] = data;
		}
	}

	_zRange = _zSize / SectorBlockSize;
	_xRange = _xSize / SectorBlockSize;

	_sectors.resize(_zRange * _xRange + 1);

	// Sector 초기화
	for (int32 i = 0; i < _zRange * _xRange; i++)
		_sectors[i] = new Sector();

	// BoundBox boundTestBox { 80, 80,64,64};
	BoundBox boundBox1 { 175, 144,147,107};
	BoundBox boundBox2 { 91, 217,48,174};
	
	// _spawnZones.push_back(new SpawnZone(0, 1, boundTestBox, MonsterType::Bear));
	_spawnZones.push_back(new SpawnZone(0, 5, boundBox1, MonsterType::Bear));
	_spawnZones.push_back(new SpawnZone(1, 5, boundBox1, MonsterType::Skeleton));
	_spawnZones.push_back(new SpawnZone(2, 5, boundBox1, MonsterType::Thief));

	_spawnZones.push_back(new SpawnZone(3, 5, boundBox2, MonsterType::Bear));
	_spawnZones.push_back(new SpawnZone(4, 5, boundBox2, MonsterType::Skeleton));
	_spawnZones.push_back(new SpawnZone(5, 5, boundBox2, MonsterType::Thief));

	//_spawnZones.push_back(new SpawnZone(3, 10, boundBox2, MonsterType::Bear));
	//_spawnZones.push_back(new SpawnZone(4, 10, boundBox2, MonsterType::Skeleton));
	//_spawnZones.push_back(new SpawnZone(5, 10, boundBox2, MonsterType::Thief));

	//_spawnZones.push_back(new SpawnZone(6, 10, boundBox3, MonsterType::Bear));
	//_spawnZones.push_back(new SpawnZone(7, 10, boundBox3, MonsterType::Skeleton));
	//_spawnZones.push_back(new SpawnZone(8, 10, boundBox3, MonsterType::Thief));

	//_spawnZones.push_back(new SpawnZone(9, 10, boundBox4, MonsterType::Bear));
	//_spawnZones.push_back(new SpawnZone(10, 10, boundBox4, MonsterType::Skeleton));
	//_spawnZones.push_back(new SpawnZone(11, 10, boundBox4, MonsterType::Thief));


	_d[0] = 0;
	_d[1] = -_zRange;
	_d[2] = -_zRange + 1;
	_d[3] = +1;
	_d[4] = _zRange + 1;
	_d[5] = +_zRange;
	_d[6] = +_zRange - 1;
	_d[7] = -1;
	_d[8] = -1 - _zRange;
}

void MapManager::Update(int32 currentTick)
{
	for (auto spawnZone : _spawnZones)
		spawnZone->Update(currentTick);
}

bool MapManager::CanGo(int32 z, int32 x)
{
	if (z >= _zSize - 1)
		return false;

	if (x >= _xSize - 1)
		return false;

	if (z < 0)
		return false;

	if (x < 0)
		return false;

	if (_closeSearch[z][x] == 1)
		return false;

	return true;
}

bool MapManager::CanGo(const Pos& pos)
{
	if (pos.z >= _zSize - 1)
		return false;

	if (pos.x >= _xSize - 1)
		return false;

	if (pos.z < 0)
		return false;

	if (pos.x < 0)
		return false;

	if (_closeSearch[pos.z][pos.x] == 1)
		return false;

	return true;
}

void MapManager::Set(Creature* creature)
{
	CreatureType ct = creature->GetCreatureType();
	int32 sectorIndex = ConvertSectorIndex(creature);
	std::vector<int32> adjacent;
	ConvertSectorIndexAdjacentIndex(sectorIndex, adjacent);
	switch (ct)
	{
	case PLAYER:
	{
		GameSession* session = static_cast<Player*>(creature)->GetSession();
		_sectors[sectorIndex]->Set(session);

		for (auto sector : adjacent)
		{
			_sectors[sector]->SendPlayerList(session);
			_sectors[sector]->SendMonsterList(session);
		}
	}
		break;

	case MONSTER:
	{
		Monster* monster = static_cast<Monster*>(creature);
		_sectors[sectorIndex]->Set(monster);

		for (auto sector : adjacent)
		{
			_SpawnMonster(sector, monster);
		}
	}
		break;
	}
}

void MapManager::ReSet(Creature* creature)
{
	CreatureType ct = creature->GetCreatureType();
	int32 sectorIndex = ConvertSectorIndex(creature);
	std::vector<int32> adjacent;
	ConvertSectorIndexAdjacentIndex(sectorIndex, adjacent);
	switch (ct)
	{
	case PLAYER:
	{
		GameSession* session = static_cast<Player*>(creature)->GetSession();
		_sectors[sectorIndex]->Reset(session);

		for (auto sector : adjacent)
		{
			_sectors[sector]->SendPlayerRemoveList(session);
			_sectors[sector]->SendMonsterRemoveList(session);
		}
	}
	break;

	case MONSTER:
	{
		Monster* monster = static_cast<Monster*>(creature);
		_sectors[sectorIndex]->Reset(monster);

		for (auto sector : adjacent)
		{
			_UnSpawnMonster(sector, monster);
		}
	}
		break;
	}
}

void MapManager::MapSync(Creature* creature)
{
	CreatureType ct = creature->GetCreatureType();
	
	Vector3 nowPos = creature->GetPos();
	Vector3 prevPos = creature->GetPrevPos();
	
	int32 sectorIndex = ConvertSectorIndex(nowPos);
	int32 prevSectorIndex = ConvertSectorIndex(prevPos);

	if (prevSectorIndex == sectorIndex) return;

	switch (ct)
	{
	case PLAYER:
	{
		GameSession* session = static_cast<Player*>(creature)->GetSession();
		
		_sectors[prevSectorIndex]->Reset(session);
		_sectors[sectorIndex]->Set(session);

		std::vector<int32> nowAdjacent;
		std::vector<int32> prevAdjacent;
		ConvertSectorIndexAdjacentIndex(sectorIndex, nowAdjacent);
		ConvertSectorIndexAdjacentIndex(prevSectorIndex, prevAdjacent);

		std::set<int32> nowSet;
		std::set<int32> prevSet;

		std::vector<int32>  remove;
		std::vector<int32>  adds;

		for (auto now : nowAdjacent)
			nowSet.insert(now);

		for (auto prev : prevAdjacent)
			prevSet.insert(prev);

		for (auto prev : prevSet)
		{
			if (!nowSet.count(prev))
				remove.push_back(prev);
		}

		for (auto now : nowSet)
		{
			if (!prevSet.count(now))
				adds.push_back(now);
		}

		for (auto item : remove)
		{
			_sectors[item]->SendPlayerRemoveList(session);
			_sectors[item]->SendMonsterRemoveList(session);
		}

		for (auto item : adds)
		{
			_sectors[item]->SendPlayerList(session);
			_sectors[item]->SendMonsterList(session);
		}
	}
	break;

	case MONSTER:
	{
		Monster* monster = static_cast<Monster*>(creature);

		_sectors[prevSectorIndex]->Reset(monster);
		_sectors[sectorIndex]->Set(monster);

		std::vector<int32> nowAdjacent;
		std::vector<int32> prevAdjacent;
		ConvertSectorIndexAdjacentIndex(sectorIndex, nowAdjacent);
		ConvertSectorIndexAdjacentIndex(prevSectorIndex, prevAdjacent);

		std::set<int32> nowSet;
		std::set<int32> prevSet;

		std::vector<int32>  remove;
		std::vector<int32>  adds;

		for (auto now : nowAdjacent)
			nowSet.insert(now);

		for (auto prev : prevAdjacent)
			prevSet.insert(prev);

		for (auto prev : prevSet)
		{
			if (!nowSet.count(prev))
				remove.push_back(prev);
		}

		for (auto now : nowSet)
		{
			if (!prevSet.count(now))
				adds.push_back(now);
		}

		for (auto item : remove)
		{
			_UnSpawnMonster(item, monster);
		}

		for (auto item : adds)
		{
			_SpawnMonster(item, monster);
		}
	}
		break;
	}
}

void MapManager::BroadCast(Creature* creature, BYTE* sendBuffer, int32 sendSize)
{
	int32 hereIndex = ConvertSectorIndex(creature->GetPos());

	std::vector<int32> adjacent;
	ConvertSectorIndexAdjacentIndex(hereIndex, adjacent);

	CreatureType ct = creature->GetCreatureType();

	switch (ct)
	{
	case PLAYER:
		for (auto item : adjacent)
			_sectors[item]->BroadCast(static_cast<Player*>(creature)->GetSession(), sendBuffer, sendSize);
		break;
	case MONSTER:
		for (auto item : adjacent)
			_sectors[item]->BroadCast(nullptr, sendBuffer, sendSize);
		break;
	}
}

void MapManager::ReSpawn(Monster* monster)
{
	int32 spawnPos = monster->GetSpawnPos();
	_spawnZones[spawnPos]->DecreaseSpawnCnt();
}

void MapManager::FindPath(const Vector3& dest, const Vector3 start, std::vector<Pos>& path)
{
	Pos startPos = { static_cast<int32>(start.x), static_cast<int32>(start.z) };
	Pos endPos = { static_cast<int32>(dest.x), static_cast<int32>(dest.z) };

	if (CanGo(endPos) == false)
		return;

	Pos front[8] =
	{
		{0, 1}, // 위
		{1, 0}, // 오른쪽
		{0, -1}, // 아래쪽
		{-1, 0}, //왼쪽
		{1, 1}, // 위 오른쪽
		{-1, 1}, // 위 왼쪽
		{1, -1}, // 아래 오른쪽
		{-1,-1} // 아래 왼쪽
	};

	int32 cost[8] =
	{
		10,
		10,
		10,
		10,
		14,
		14,
		14,
		14
	};

	int32 dirCount = 8;

	std::vector<std::vector<int32>> best(_zSize + 1, std::vector<int32>(_xSize + 1, INT32_MAX));

	std::priority_queue<PQNode, std::vector<PQNode>, std::greater<PQNode>> pq;

	std::map<Pos, Pos> parent;

	{
		int32 g = 0;
		int32 h = 10 * (abs(endPos.z - endPos.z) + abs(startPos.z - startPos.x));
		pq.push({ g + h, g, startPos });
		best[startPos.z][startPos.x] = g + h;
		parent[startPos] = startPos;
	}

	while (pq.empty() == false)
	{
		PQNode node = pq.top();
		pq.pop();

		Pos pos = node.pos;

		if (best[pos.z][pos.x] < node.f)
			continue;

		if (node.pos == endPos)
			break;

		for (int dir = 0; dir < dirCount; dir++)
		{
			Pos nextPos = pos + front[dir];

			if (CanGo(nextPos) == false) continue;

			int nextG = node.g + cost[dir];
			int nextH = 10 * (abs(endPos.z - nextPos.z) + abs(endPos.x - nextPos.x));

			if (best[nextPos.z][nextPos.x] <= nextG + nextH) continue;

			pq.push({ nextG + nextH, nextG, nextPos });
			best[nextPos.z][nextPos.x] = nextG + nextH;
			parent[nextPos] = pos;
		}
	}

	Pos pos = endPos;

	while (true)
	{
		path.push_back(pos);
		pos = parent[pos];
		if (pos == startPos)
			break;
	}

	std::reverse(path.begin(), path.end());
}

void MapManager::_SpawnMonster(int32 index, Monster* monster)
{
	Vector3 monsterV3 = monster->GetPos();

	BYTE sendBuffer[256] = {};
	BufferWriter bw(sendBuffer);

	PacketHeader* header = bw.WriteReserve<PacketHeader>();
	State monsterState = monster->GetState();

	if (monsterState == PATROL || monsterState == TRACE)
		monsterState = MOVE;

	bw.Write(monsterState);
	bw.Write(monster->GetMonsterId());
	bw.Write(monster->GetMonsterType());
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

	header->_pktSize = bw.GetWriterSize();
	header->_type = S2C_MONSTERSPAWN;

	_sectors[index]->BroadCast(nullptr, sendBuffer, header->_pktSize);
}

void MapManager::_UnSpawnMonster(int32 index, Monster* monster)
{
	BYTE sendBuffer[4096] = {};
	BufferWriter bw(sendBuffer);

	PacketHeader* header = bw.WriteReserve<PacketHeader>();
	bw.Write(monster->GetMonsterId());

	header->_pktSize = bw.GetWriterSize();
	header->_type = S2C_MONSTERDEAD;

	_sectors[index]->BroadCast(nullptr, sendBuffer, header->_pktSize);
}

int32 MapManager::ConvertSectorIndex(Creature* creature)
{
	Vector3 pos = creature->GetPos();
	int32 x = static_cast<int32>(pos.x);
	int32 z = static_cast<int32>(pos.z);

	const int32 sx = x / SectorBlockSize;
	const int32 sz = z / SectorBlockSize;

	int32 ret = (sz * _xRange) + sx;

	return ret;
}

int32 MapManager::ConvertSectorIndex(const Vector3& v3)
{
	int32 x = static_cast<int32>(v3.x);
	int32 z = static_cast<int32>(v3.z);

	const int32 sx = x / SectorBlockSize;
	const int32 sz = z / SectorBlockSize;

	int32 ret = (sz * _xRange) + sx;

	return ret;
}

void MapManager::ConvertSectorIndexAdjacentIndex(int32 index, std::vector<int32>& out)
{
	for (int i = 0; i < ADJACENT_COUNT; i++)
	{
		int32 nextIndex = index + _d[i];

		if (nextIndex < 0 || nextIndex >= _xRange * _zRange)
			continue;

		out.push_back(nextIndex);
	}
}