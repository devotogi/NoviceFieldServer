#include "pch.h"
#include "MonsterTable.h"
#include "Monster.h"
MonsterTable::MonsterTable()
{
}

MonsterTable::~MonsterTable()
{
}

void MonsterTable::Init()
{
	_tables[MonsterType::Bear] = {2000, 100,1.5f,250, 600, false, 2000};
	_tables[MonsterType::Skeleton] = {1000,10,2.0f,500,500, false, 1000};
	_tables[MonsterType::Thief] = {9000,1000,2.5f,300,800, false, 500};
}

void MonsterTable::SetMonsterAttribute(Monster* monster)
{
	monster->SetAttribute(_tables[monster->GetMonsterType()]);
}
