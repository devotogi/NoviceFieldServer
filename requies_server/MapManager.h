#pragma once
class GameObject;
class Sector;
class SpawnZone;
class Creature;
class GameSession;
class Monster;

class MapManager
{
private:
	int32											_zSize;
	int32											_xSize;
	int32											_xRange;
	int32											_zRange;
	std::vector<std::vector<int32>>					_closeSearch;	// 갈수 있는 지형, 갈수 없는 지향 확인하는 데이터
	std::vector<std::vector<std::set<GameObject*>>> _objectSearch;  // 영역에 플레이어가 있는지 확인하는 데이터
	std::vector<Sector*>							_sectors;		// Sector
	std::vector<SpawnZone*>							_spawnZones;	// SpawnZones
	CRITICAL_SECTION								_cs;
	
	// 자기자신, 위, 오른위, 오른, 오른아래, 아래, 왼아래, 왼, 왼위
	int32 _d[9] = { 0,0,0,0,0,0,0,0,0 };

public:
	static MapManager* GetInstance()
	{
		static MapManager instance;
		return &instance;
	}

	MapManager()
	{
		InitializeCriticalSection(&_cs);
	}

	~MapManager()
	{
		DeleteCriticalSection(&_cs);
	}

	void MapLoad(const char* filePath);
	void Update(int32 currentTick);
	bool CanGo(int32 z, int32 x);
	bool CanGo(const Pos& pos);
	void Set(Creature* creature);
	void ReSet(Creature* creature);
	void MapSync(Creature* creature);
	int32 ConvertSectorIndex(Creature* creature);
	int32 ConvertSectorIndex(const Vector3& v3);
	void ConvertSectorIndexAdjacentIndex(int32 index, std::vector<int32>& out);
	void BroadCast(Creature* creature, BYTE* sendBuffer, int32 sendSize);
	void ReSpawn(Monster* monster);
	void FindPath(const Vector3& dest, const Vector3 start, std::vector<Pos>& path);
	std::vector<Sector*>& GetSectors() { return _sectors; }
private:
	void _SpawnMonster(int32 index, Monster* monster);
	void _UnSpawnMonster(int32 index, Monster* monster);
};

