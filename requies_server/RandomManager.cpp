#include "pch.h"
#include "RandomManager.h"

int32 RandomManager::GetRandomDir()
{
    return _dirRandom(_generator);
}

int32 RandomManager::GetRandomPateon()
{
    return _paeteonRandom(_generator);
}

bool RandomManager::GetRandom10()
{
    if (9 == _tenRandom(_generator))
        return true;
    else
        return false;
}
