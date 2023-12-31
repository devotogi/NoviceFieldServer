#pragma once
class MonsterTable
{
private:
	MonsterAttribute _tables[MonsterType::MonstrerTypeMax];

public:
	static MonsterTable* GetInstnace()
	{
		static MonsterTable _monsterTable;
		return &_monsterTable;
	}


	MonsterTable();
	~MonsterTable();
	
	void Init();
	void SetMonsterAttribute(class Monster* monster);
};

