#include "pch.h"
#include "Player.h"
#include "GameSession.h"
#include "BufferWriter.h"
#include "MapManager.h"
Player::Player(GameSession* session, int32 sessionId, const Vector3& pos, WCHAR* playerName, int32 level, int32 hp, int32 mp, int32 damage, float speed, float defense, int32 playerType, int32 playerSQ, int32 exp) : _sessionId(sessionId), _mouseDir(Dir::NONE), _level(level), _cameraLocalRotation({ 0,0,0,1 }), _session(session), Creature(CreatureType::PLAYER, pos, pos, State::IDLE, Dir::NONE, hp, mp, damage, speed, false, defense, hp, mp), _playerType(playerType), _playerSQ(playerSQ), _exp(exp)
{
	SetPlayerName(playerName);
	InitializeCriticalSection(&_cs);
}

Player::~Player()
{
	EnterCriticalSection(&_cs);
	MapManager::GetInstance()->ReSet(this);
	LeaveCriticalSection(&_cs);
	DeleteCriticalSection(&_cs);
}

void Player::PlayerSync(const Vector3& pos, State state, Dir dir, Dir mousedir, const Quaternion& cameraLocalRotation, const Vector3& target, MoveType moveType)
{
	EnterCriticalSection(&_cs);
	_pos = pos;
	_state = state;
	_dir = dir;
	_cameraLocalRotation = cameraLocalRotation;
	_mouseDir = mousedir;
	_target = target;
	_moveType = moveType;
	LeaveCriticalSection(&_cs);
}

bool Player::Attacked(Creature* Attacker, int32 damage)
{
	if (_death) return _death;
	int trueDamage = damage - _defensive;
	_hp -= trueDamage;

	if (_hp <= 0) _hp = 0;

	// TODO 죽었는지 처리
	BYTE sendBuffer[100];
	BufferWriter bw(sendBuffer);
	PacketHeader* pktHeader = bw.WriteReserve<PacketHeader>();

	bw.Write(_sessionId);
	bw.Write(_hp);
	bw.Write(_mp);
	bw.Write(trueDamage);

	pktHeader->_type = PacketProtocol::S2C_PLAYERATTACKED;
	pktHeader->_pktSize = bw.GetWriterSize();

	MapManager::GetInstance()->BroadCast(_session->GetPlayer(), sendBuffer, bw.GetWriterSize());

	if (_hp == 0) 
	{
		_death = true;
		// 플레이어 죽음
		BYTE sendBuffer[100];
		BufferWriter bw(sendBuffer);
		PacketHeader* pktHeader = bw.WriteReserve<PacketHeader>();

		bw.Write(_sessionId);
		pktHeader->_type = PacketProtocol::S2C_PLAYERDETH;
		pktHeader->_pktSize = bw.GetWriterSize();

		MapManager::GetInstance()->BroadCast(_session->GetPlayer(), sendBuffer, bw.GetWriterSize());
	}

	return _death;
}

void Player::ReSpawn()
{
	MapManager::GetInstance()->ReSet(this);

	EnterCriticalSection(&_cs);
	_pos = { 6,0,125 };
	_prevPos = { 6,0,125 };
	_state = IDLE;
	_dir = NONE;
	_mouseDir = NONE;
	LeaveCriticalSection(&_cs);

	BYTE sendBuffer[100];
	BufferWriter bw(sendBuffer);
	PacketHeader* pktHeader = bw.WriteReserve<PacketHeader>();

	int32 sessionId = _sessionId;
	uint16 playerState = (uint16)_state;
	uint16 playerDir = (uint16)_dir;
	uint16 playerMouseDir = (uint16)_mouseDir;
	Vector3 playerPos = _pos;
	Quaternion playerQuaternion = _cameraLocalRotation;
	_hp = _hpMax;
	_mp = _mpMax;
	float hp = _hp;
	float mp = _mp;

	bw.Write(sessionId);
	bw.Write(playerState);
	bw.Write(playerDir);
	bw.Write(playerMouseDir);
	bw.Write(playerPos);
	bw.Write(playerQuaternion);
	bw.Write(hp);
	bw.Write(mp);
	_death = false;
	pktHeader->_type = PacketProtocol::S2C_PLAYERESPAWN;
	pktHeader->_pktSize = bw.GetWriterSize();

	_session->Send(sendBuffer, pktHeader->_pktSize);
	MapManager::GetInstance()->Set(this);
}

void Player::ExpUp(float exp)
{
	EnterCriticalSection(&_cs);
	_exp += exp;
	if (_exp >= _expMax) 
	{
		LevelUp();
		SendStatInfo(); 
	}
	LeaveCriticalSection(&_cs);
	BYTE sendBuffer[100] = {};
	BufferWriter bw(sendBuffer);
	PacketHeader* pktHeader = bw.WriteReserve<PacketHeader>();
	bw.Write(_sessionId);
	bw.Write(_level);
	bw.Write(_exp);
	bw.Write(_expMax);
	bw.Write(_hp);
	pktHeader->_type = PacketProtocol::S2C_PLAYEREXP;
	pktHeader->_pktSize = bw.GetWriterSize();
	MapManager::GetInstance()->BroadCast(this,sendBuffer, bw.GetWriterSize());
}

void Player::LevelUp()
{
	_level++;
	_hpMax = _hpMax + (_level * (_hpMax * 0.2));
	_hp = _hpMax;
	StatPointUp();
}

int8 Player::GetLevel()
{
	return _level; // _level;
}

float Player::GetSpeed()
{
	return _speed;
}

float Player::GetDamage()
{
	return _damage;
}

void Player::StatPointUp()
{
	EnterCriticalSection(&_cs);
	_statPoint += 4;
	LeaveCriticalSection(&_cs);
}
	
void Player::StatPointDown()
{
	EnterCriticalSection(&_cs);
	_statPoint--;

	if (_statPoint <= 0) _statPoint = 0;
	LeaveCriticalSection(&_cs);
}

void Player::SendStatInfo()
{
	BYTE sendBuffer[1000];
	BufferWriter bw(sendBuffer);
	PacketHeader* pktHeader = bw.WriteReserve<PacketHeader>();

	bw.Write(_damage);
	bw.Write(_speed);
	bw.Write(_defensive);
	bw.Write(_statPoint);

	pktHeader->_type = PacketProtocol::S2C_PLAYERSTATINFO;
	pktHeader->_pktSize = bw.GetWriterSize();
	_session->Send(sendBuffer, bw.GetWriterSize());
}

void Player::UpStat(int32 type)
{
	if (_statPoint == 0) return;

	switch (type)
	{
	case 0:
		_damage += 10;
		break;

	case 1:
		_speed += 0.1;
		break;

	case 2:
		_defensive += 1;
		break;
	}
	StatPointDown();
}

void Player::SetPlayerName(WCHAR* playerName)
{
	int32 size = wcslen(playerName) * sizeof(WCHAR);
	::memcpy(_playerName, playerName, size);
}
