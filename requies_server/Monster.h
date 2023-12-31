#pragma once
#include "Creature.h"
#include "LockBasedQueue.h"
class Monster : public Creature
{
	enum : int32
	{
		ATTACKED_TICK = 1000,
		COOL_TIME_TICK = 1000,
		MOVE_TICK = 400,
		SEARCH_TICK = 1000,
		PATROL_TICK = 2000,
		IDLE_TICK = 1000,
	};

private:
	int32								_spawnZoneIndex;
	MonsterType							_monsterType;
	Vector3								_vDir;
	int32								_monsterId;
	Creature*							_target = nullptr;
	Vector3								_dest = {0,0,0};
	std::vector<Pos>					_path;
	std::vector<Pos>					_conner;
	std::queue<int32>					_damageQueue;
	LockBasedQueue<MonsterAttackedInfo> _monsterAttackedEventQueue;
	float								_exp = 0;
	bool								_firstAttack = false;

private:
	Creature* _attacker = nullptr;
	int32	  _attackedDamage = 0;

public:
	Monster(int32 spawnZoneIndex, int32 monsterId, MonsterType type, const Vector3& pos);
	Monster(int32 monsterId, MonsterType type);
	Monster(int32 monsterId);
	Monster();
	virtual ~Monster();

	MonsterType GetMonsterType() { return _monsterType; }
	int32 GetMonsterId() { return _monsterId; }
	virtual bool Attacked(Creature* Attacker, int32 damage);
	void Update(int32 deltaTick);
	Vector3 GetVDir() { return _vDir; }
	int32 GetSpawnPos() { return _spawnZoneIndex; }
	void AttackCheck();
	Vector3 GetDest() { return _dest; }
	std::vector<Pos>& GetConner() { return _conner; }
	bool IsDead() { return _death; }
	void Spawn(const Vector3& nowPos, const Vector3& prevPos, int32 spawnIndex);
	void Clear();
	void SetAttribute(const MonsterAttribute& attr);
	void SetMonsterType(MonsterType type) { _monsterType = type; }

private:
	void UpdateIdle(int32 deltaTcik);
	void UpdateAttack(int32 deltaTcik);
	void UpdateAttacked(int32 deltaTcik);
	void UpdateDeath(int32 deltaTcik);
	void UpdateCoolTime(int32 deltaTcik);
	void UpdatePatrol(int32 deltaTcik);
	void UpdateTrace(int32 deltaTcik);
	void SyncMonsterPacket();

private:
	void AttackedProc();

private:
	void PatrolDestSearch();
	void TraceSearch();

private:
	int32	_idleLastTick = 0;
	int32   _idleSumTick = 0;

	int32   _patrolLastTick = 0;
	int32   _patrolSumTick = 0;

	int32	_attackedLastTick = 0;
	int32	_attackedSumTick = 0;

	int32	_coolTimeLastTick = 0;
	int32	_coolTimeSumTick = 0;

};

