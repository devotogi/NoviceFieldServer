#pragma once
#include "Creature.h"
class GameSession;

class Player : public Creature
{
private:
	int32				_playerSQ;
	int32				_sessionId;
	Dir					_mouseDir;
	Vector3				_target;
	MoveType			_moveType;
	Quaternion			_localRotation;
	Quaternion			_cameraLocalRotation;
	GameSession*		_session;
	CRITICAL_SECTION	_cs;
	float				_exp = 0;
	float				_expMax = 1000.f;
	int8				_level = 1;
	int32				_statPoint = 0;
	int8				_playerType;
	WCHAR				_playerName[256] = {};
	Vector3				_angle;

public:
	Player(GameSession* session, int32 sessionId, const Vector3& pos, WCHAR* playerName, int32 level, int32 hp, int32 mp, int32 damage, float speed, float defense, int32 playerType, int32 playerSQ, int32 exp);
	~Player();

	Dir			 GetMouseDir() { return _mouseDir; }
	Quaternion&  GetCameraLocalRotation() { return _cameraLocalRotation; }
	MoveType	 GetMoveType() { return _moveType; }
	Vector3&	 GetTarget() { return _target; }
	Quaternion&	 GetLocalRtation() { return _localRotation; }
	void		 PlayerSync(const Vector3& pos, State state, Dir dir, Dir mousedir, const Quaternion& cameraLocalRotation, const Vector3& target, MoveType moveType, const Vector3 angle);
	virtual bool Attacked(Creature* Attacker, int32 damage);
	void         ReSpawn();
	GameSession* GetSession() { return _session; }
	void		 ExpUp(float exp);
	void		 LevelUp();
	int8		 GetLevel();

	float        GetSpeed();
	float        GetDamage();
	int32		 GetExp() { return _exp; }
public:
	void         StatPointUp();
	void         StatPointDown();
	void         SendStatInfo();
	void		 UpStat(int32 type);
	void		 SetPlayerName(WCHAR* playerName);
	WCHAR*		 GetPlayerName() { return _playerName; }
	int8		 GetPlayerType() { return _playerType; }
	void		 SetPlayerSQ(int32 playerSQ) { _playerSQ = playerSQ; }
	int32		 GetPlayerSQ() { return _playerSQ; }
};