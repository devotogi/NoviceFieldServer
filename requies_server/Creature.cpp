#include "pch.h"
#include "Creature.h"

Creature::Creature(
	CreatureType creatureType,
	const Vector3& pos,
	const Vector3& prevPos,
	State state,
	Dir dir,
	float hp,
	float mp,
	float damage,
	float speed,
	bool death,
	float defensive,
	float hpMax,
	float mpMax
) : 
	_creatureType(creatureType),
	_pos(pos),
	_prevPos(prevPos),
	_state(state),
	_dir(dir),
	_hp(hp),
	_mp(mp),
	_damage(damage),
	_speed(speed), 
	_death(death),
	_defensive(defensive),
	_hpMax(hpMax),
	_mpMax(mpMax)
{
}

Creature::Creature()
{
}

Creature::~Creature()
{
}

bool Creature::Attacked(Creature* Attacker, int32 damage)
{
	return false;
}
