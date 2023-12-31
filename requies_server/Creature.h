#pragma once
class Creature
{
protected:
	CreatureType					_creatureType;
	Vector3							_pos;
	Vector3							_prevPos;
	State							_state;
	Dir								_dir;
	float							_hp;		
	float							_mp;
	float							_hpMax;
	float							_mpMax;
	bool							_death = false;
	float							_damage;
	float							_speed;
	float							_defensive;

public:
	Creature(
		CreatureType creatureType
		, const Vector3& pos
		, const Vector3& prevPos
		, State state
		, Dir dir
		, float hp
		, float mp
		, float damage
		, float speed
		, bool death
		, float defensive
		, float hpMax
		, float mpMax
	);
	Creature();
	virtual ~Creature();

	CreatureType GetCreatureType() { return _creatureType; }
	Vector3&	 GetPos() { return _pos; }
	Vector3&	 GetPrevPos() { return _prevPos; }
	State		 GetState() { return _state; }
	Dir			 GetDir() { return _dir; }
	float		 GetHpMax() { return _hpMax; }
	float		 GetMpMax() { return _mpMax; }
	float		 GetHp() { return _hp; }
	float		 GetMp() { return _mp; }
	void		 SetPrevPos(const Vector3& prevPos) { _prevPos = prevPos; }
	void		 SetPos(const Vector3& pos) { _pos = pos; }

	virtual bool Attacked(Creature* Attacker, int32 damage);
};

