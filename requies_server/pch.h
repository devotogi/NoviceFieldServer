#pragma once
#ifdef _DEBUG
#pragma comment(lib, "Debug\\mallo.lib")
#else 
#pragma comment(lib, "Release\\mallo.lib")
#endif

#include "CorePch.h"
#include "Packet.h"
struct UserInfo
{
public:
	WCHAR userId[250] = {};
	WCHAR userPw[250] = {};
};