#include "pch.h"
#include "MonsterTable.h"
#include "Monster.h"
#include "MyDBConnection.h"
MonsterTable::MonsterTable()
{
}

MonsterTable::~MonsterTable()
{
}

void MonsterTable::Init()
{
	DBConnection* con = PlayerDBConnectionPool::GetInstance()->Pop();

	int32 monsterType;
	int32 hp;
	int32 defensive;
	float speed;
	int32 damage;
	int32 exp;
	int32 firstAttack;
	int32 level;

	SQLLEN len;
	SQLPrepare(con->GetHSTMT(), (SQLWCHAR*)L"SELECT MONSTER_TYPE, HP, DEFENSIVE, SPEED, DAMAGE, EXP, FIRST_ATTACK, LEVEL FROM player.d_monsterInfo;", SQL_NTS);

	SQLBindCol(con->GetHSTMT(), 1, SQL_INTEGER, &monsterType, sizeof(monsterType), &len);
	SQLBindCol(con->GetHSTMT(), 2, SQL_INTEGER, &hp, sizeof(hp), &len);
	SQLBindCol(con->GetHSTMT(), 3, SQL_INTEGER, &defensive, sizeof(defensive), &len);
	SQLBindCol(con->GetHSTMT(), 4, SQL_C_FLOAT, &speed, sizeof(speed), &len);
	SQLBindCol(con->GetHSTMT(), 5, SQL_INTEGER, &damage, sizeof(damage), &len);
	SQLBindCol(con->GetHSTMT(), 6, SQL_INTEGER, &exp, sizeof(exp), &len);
	SQLBindCol(con->GetHSTMT(), 7, SQL_INTEGER, &firstAttack, sizeof(firstAttack), &len);
	SQLBindCol(con->GetHSTMT(), 8, SQL_INTEGER, &level, sizeof(level), &len);
	SQLExecute(con->GetHSTMT());
	while (SQLFetch(con->GetHSTMT()) != SQL_NO_DATA)
	{
		bool firstAttackbool;
		if (firstAttack == 1)
			firstAttackbool = true;
		else
			firstAttackbool = false;

		_tables[monsterType] = { hp, defensive,speed,damage, exp, firstAttackbool};
	}	
	SQLCloseCursor(con->GetHSTMT());
	PlayerDBConnectionPool::GetInstance()->Push(con);
}

void MonsterTable::SetMonsterAttribute(Monster* monster)
{
	monster->SetAttribute(_tables[monster->GetMonsterType()]);
}
