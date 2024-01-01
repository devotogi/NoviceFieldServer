#include "pch.h"
#include "ServerService.h"
#include "GameSession.h"
#include "ThreadManager.h"
#include "IOCPCore.h"
#include "SpawnZone.h"
#include "MapManager.h"
#include "MonsterManager.h"
#include "MonsterTable.h"
#include "MyDBConnection.h"

unsigned int _stdcall DispatchProc(void* Args)
{
	ServerService* service = reinterpret_cast<ServerService*>(Args);
	while (true)
		service->GetIOCPCore()->Dispatch();

	return 0;
}

unsigned int _stdcall AcceptProc(void* Args)
{
	ServerService* service = reinterpret_cast<ServerService*>(Args);
	service->Start();

	return 0;
}

void Update(int32 currentTick) 
{
	MapManager::GetInstance()->Update(currentTick);
	MonsterManager::GetInstnace()->Update(currentTick);
}


int main()
{
	PlayerDBConnectionPool::GetInstance()->Init(L"PLAYER", L"sa", L"root", 5);
	AccountDBConnectionPool::GetInstance()->Init(L"MSSQL", L"sa", L"root", 5);
	MonsterTable::GetInstnace()->Init();
	MonsterManager::GetInstnace()->Init(1000);
	MapManager::GetInstance()->MapLoad("C:\\Users\\jgkang\\Desktop\\map\\map.dat");

	const char* ip = "58.236.130.58";
	ServerService service(ip, 30002, GameSession::MakeGameSession);

	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);

	int32 threadCount = sysInfo.dwNumberOfProcessors * 2;

	for (int i = 0; i < threadCount; i++)
		ThreadManager::GetInstance()->Launch(DispatchProc, &service);

	ThreadManager::GetInstance()->Launch(AcceptProc, &service);
	
	while (true)
	{
		int32 currentTick = ::GetTickCount64();
		Update(currentTick);
		Sleep(200);
	}

	ThreadManager::GetInstance()->Join();

	return 0;
}