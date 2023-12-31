#pragma once
class RandomManager
{
private:
	std::default_random_engine _generator;
	std::uniform_int_distribution<int32> _dirRandom;
	std::uniform_int_distribution<int32> _paeteonRandom;
	std::uniform_int_distribution<int32> _tenRandom;

public:
	static RandomManager* GetInstance() 
	{
		static RandomManager randomManager;
		return &randomManager;
	}

	RandomManager()
	{
		_dirRandom = std::uniform_int_distribution<int32>(0, 3);
		_paeteonRandom = std::uniform_int_distribution<int32>(0, 1);
		_tenRandom = std::uniform_int_distribution<int32>(0, 10);
	}

	~RandomManager()
	{
	
	}

	int32 GetRandomDir();
	int32 GetRandomPateon();
	bool  GetRandom10();
};

