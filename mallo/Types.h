#pragma once
using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;
using wchar = wchar_t;

enum MoveType : int8
{
	KeyBoard,
	Mouse
};

enum State : int8
{
	IDLE,
	MOVE,
	ATTACK,
	ATTACKED,
	DEATH,
	STATE_NONE,
	COOL_TIME,
	PATROL,
	TRACE,
	RESPAWN,
};

enum Dir : int8
{
	NONE = 0,
	UP = 2,
	RIGHT = 4,
	DOWN = 8,
	LEFT = 16,
	UPRIGHT = 6,
	RIGHTDOWN = 12,
	LEFTDOWN = 24,
	LEFTUP = 18,
};

enum PacketProtocol : __int16
{
	C2S_PLAYERINIT,
	S2C_PLAYERINIT,
	C2S_PLAYERSYNC,
	S2C_PLAYERSYNC,
	S2C_PLAYERLIST,
	S2C_PLAYERREMOVELIST,
	S2C_PLAYERENTER,
	S2C_PLAYEROUT,
	C2S_LATENCY,
	S2C_LATENCY,
	C2S_MAPSYNC,
	S2C_MAPSYNC,
	S2C_PLAYERNEW,
	S2C_PLAYERDESTORY,
	C2S_PLAYERATTACK,
	S2C_PLAYERATTACKED,
	C2S_PLAYERCHAT,
	S2C_PLAYERCHAT,
	S2C_PLAYERDETH,
	C2S_PLAYERESPAWN,
	S2C_PLAYERESPAWN,
	S2C_MONSTERSPAWN,
	S2C_MONSTERREMOVELIST,
	S2C_MONSTERRENEWLIST,
	C2S_MONSTERATTACKED,
	S2C_MONSTERATTACKED,
	S2C_MONSTERDEAD,
	S2C_MONSTERSYNC,
	S2C_NEWMONSTER,
	S2C_DELETEMONSTER,
	S2C_MONSTERDEADCLIENT,
	S2C_MONSTERINFO,
	S2C_PLAYEREXP,
	C2S_PLAYERSTATINFO,
	S2C_PLAYERSTATINFO,
	C2S_UPSTAT,
	C2S_LOGIN,
	S2C_LOGIN,
	C2S_CREATECHARACTER,
	S2C_CREATECHARACTER,
	S2C_CHARACTERLIST,
	C2S_CHARACTERLIST,
	C2S_DELETECHARACTER,
	C2S_GAMEPLAY,
	S2C_SERVERMOVE,
	C2S_SERVER_MOVE,
	C2S_PLAYERSKILLSYNC,
};

struct Vector3
{
	float x = 0;
	float y = 0;
	float z = 0;

	Vector3 operator*(float d)
	{
		return { x * d, y * d, z * d };
	}

	bool operator==(const Vector3& d)
	{
		return x == d.x && y == d.y && z == d.z;
	}

	bool operator!=(const Vector3& d)
	{
		return !(*this == d);
	}

	Vector3 operator+(const Vector3& d)
	{
		return { x + d.x, y + d.y, z + d.z };
	}

	Vector3 operator-(const Vector3& d)
	{
		return { x - d.x, y - d.y, z - d.z };
	}

	float Magnitude()
	{
		return sqrt((x * x) + (y * y) + (z * z));
	}

	Vector3 Normalized()
	{
		return Vector3{ x / Magnitude(), y / Magnitude(), z / Magnitude() };
	}

	bool Zero()
	{
		return x == 0 && y == 0 && z == 0;
	}
};


struct Pos
{
	int x = 0;
	int z = 0;

	Pos operator+(Pos& other)
	{
		Pos ret;
		ret.x = x + other.x;
		ret.z = z + other.z;

		return ret;
	}

	Pos operator-(Pos& other)
	{
		Pos ret;
		ret.x = x - other.x;
		ret.z = z - other.z;

		return ret;
	}

	Vector3 operator-(const Vector3& other)
	{
		return Vector3 { x - other.x, other.y, z - other.z };
	}

	Pos& operator+=(Pos& other)
	{
		x += other.x;
		z += other.z;

		return *this;
	}

	bool operator==(Pos& other)
	{
		return z == other.z && x == other.x;
	}

	bool operator!=(Pos& other)
	{
		return !(*this == other);
	}

	bool operator<(const Pos& other) const
	{
		if (z != other.z)
			return z < other.z;
		return x < other.x;
	}

	bool operator>(const Pos& other) const
	{
		if (z != other.z)
			return z > other.z;
		return x > other.x;
	}
};


struct PacketHeader
{
	__int16 _type = 0;
	__int16 _pktSize = 0;
};

struct Quaternion
{
	float x;
	float y;
	float z;
	float w;
};

struct BoundBox 
{
public:
	int maxX = 0;
	int maxZ = 0;
	int minX = 0;
	int minZ = 0;
};

enum MonsterType : int8
{
	Bear,
	Skeleton,
	Thief,
	MonstrerTypeMax
};

enum CreatureType : int32
{
	PLAYER,
	MONSTER,
};

struct PQNode
{
	bool operator<(const PQNode& other) const { return f < other.f; }
	bool operator>(const PQNode& other) const { return f > other.f; }

	int f;
	int g;
	Pos pos;
};

struct MonsterAttackedInfo 
{
public:
	class Creature* attacker;
	int32 damage;
};

struct MonsterAttribute 
{
public:
	int hp = 0;
	int defensive = 0;
	float speed = 0;
	int damage = 0;
	int exp = 0;
	bool  firstAttack = false;
};