#include "pch.h"
#include "PacketHandler.h"
#include "Player.h"
#include "GameSession.h"
#include "BufferReader.h"
#include "BufferWriter.h"
#include "SessionManager.h"
#include "MapManager.h"
#include "MonsterManager.h"
#include "SessionManager.h"
void PacketHandler::HandlePacket(GameSession* session, BYTE* packet, int32 packetSize)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(packet);
	BYTE* dataPtr = packet + sizeof(PacketHeader);
	int32 dataSize = packetSize - sizeof(PacketHeader);

	switch (header->_type)
	{
	case PacketProtocol::C2S_PLAYERSYNC: // 플레이어 동기화
		HandlePacket_C2S_PLAYERSYNC(session, dataPtr, dataSize);
		break;

	case PacketProtocol::C2S_MAPSYNC: // 플레이어가 1칸마다 좌표 동기화 
		HandlePacket_C2S_MAPSYNC(session, dataPtr, dataSize);
		break;

	case PacketProtocol::C2S_LATENCY: // 모니터 레이턴시
		HandlePacket_C2S_LATENCY(session, dataPtr, dataSize);
		break;

	case PacketProtocol::C2S_PLAYERATTACK:
		HandlePacket_C2S_PLAYERATTACK(session, dataPtr, dataSize);
		break;

	case PacketProtocol::C2S_PLAYERCHAT:
		HandlePacket_C2S_PLAYERCHAT(session, dataPtr, dataSize);
		break;

	case PacketProtocol::C2S_PLAYERESPAWN:
		HandlePacket_C2S_PLAYERESPAWN(session, dataPtr, dataSize);
		break;

	case PacketProtocol::C2S_MONSTERATTACKED:
		HandlePacket_C2S_MONSTERATTACKED(session, dataPtr, dataSize);
		break;

	case PacketProtocol::C2S_PLAYERSTATINFO:
		HandlePacket_C2S_PLAYERSTATINFO(session, dataPtr, dataSize);
		break;

	case PacketProtocol::C2S_UPSTAT:
		HandlePacket_C2S_UPSTAT(session, dataPtr, dataSize);
		break;

	case PacketProtocol::C2S_PLAYERINIT:
		HandlePacket_C2S_PLAYERINIT(session, dataPtr, dataSize);
		break;
	}
}

void PacketHandler::HandlePacket_C2S_PLAYERSYNC(GameSession* session, BYTE* packet, int32 packetSize)
{
	Player* player = session->GetPlayer();
	BufferReader br(packet);

	int32 playerId;
	State state;
	Dir dir;
	Dir mouseDir;
	Vector3 vector3;
	Quaternion quaternion;
	Vector3 target;
	MoveType moveType;

	br.Read(playerId);
	br.Read(state);
	br.Read(dir);
	br.Read(mouseDir);
	br.Read(vector3);
	br.Read(quaternion);
	br.Read(target);
	br.Read(moveType);

	player->PlayerSync(vector3, state, dir, mouseDir, quaternion, target, moveType);

	BYTE sendBuffer[100];
	BufferWriter bw(sendBuffer);
	PacketHeader* pktHeader = bw.WriteReserve<PacketHeader>();

	bw.Write(session->GetSessionID());
	bw.Write((int8)player->GetState());
	bw.Write((int8)player->GetDir());
	bw.Write((int8)player->GetMouseDir());
	bw.Write(player->GetPos());
	bw.Write(player->GetCameraLocalRotation());
	bw.Write(target);
	bw.Write((int8)moveType);

	pktHeader->_type = PacketProtocol::S2C_PLAYERSYNC;
	pktHeader->_pktSize = bw.GetWriterSize();

	MapManager::GetInstance()->BroadCast(session->GetPlayer(), sendBuffer, bw.GetWriterSize());
}

void PacketHandler::HandlePacket_C2S_LATENCY(GameSession* session, BYTE* packet, int32 packetSize)
{
	BufferReader br(packet);
	int32 lastTick;

	br.Read(lastTick);

	BYTE sendBuffer[20];
	BufferWriter bw(sendBuffer);
	PacketHeader* pktHeader = bw.WriteReserve<PacketHeader>();

	bw.Write(lastTick);
	pktHeader->_type = PacketProtocol::S2C_LATENCY;
	pktHeader->_pktSize = bw.GetWriterSize();

	session->Send(sendBuffer, bw.GetWriterSize());
}

void PacketHandler::HandlePacket_C2S_MAPSYNC(GameSession* session, BYTE* packet, int32 packetSize)
{
	Player* player = session->GetPlayer();
	Vector3 prevPos = player->GetPrevPos();
	BufferReader br(packet);

	int32 playerId;
	State state;
	Dir dir;
	Dir mouseDir;
	Vector3 vector3;
	Quaternion quaternion;
	Vector3 target;
	MoveType moveType;

	br.Read(playerId);
	br.Read(state);
	br.Read(dir);
	br.Read(mouseDir);
	br.Read(vector3);
	br.Read(quaternion);
	br.Read(target);
	br.Read(moveType);

	player->PlayerSync(vector3, state, dir, mouseDir, quaternion, target, moveType);
	MapManager::GetInstance()->MapSync(session->GetPlayer());
	player->SetPrevPos(vector3);
}

void PacketHandler::HandlePacket_C2S_PLAYERATTACK(GameSession* session, BYTE* packet, int32 packetSize)
{
	int32 otherPlayer;
	int32 damage;

	BufferReader br(packet);
	br.Read(otherPlayer);
	br.Read(damage);

	Player* AttackPlayer = session->GetPlayer();
	GameSession* AttackedSession = SessionManager::GetInstance()->GetSession(otherPlayer);

	if (AttackedSession == nullptr)
		return;

	Player* AttackedPlayer = AttackedSession->GetPlayer();
	
	Vector3 attackedPos = AttackedPlayer->GetPos();
	Vector3 attackerPos = AttackPlayer->GetPos();

	if (attackedPos.x <= attackerPos.x + 2 && attackedPos.x >= attackedPos.x - 2) 
	{
		if (attackedPos.z <= attackerPos.z + 2 && attackedPos.z >= attackedPos.z - 2)
		{
			AttackedPlayer->Attacked(session->GetPlayer(), damage);
		}
	}
}

void PacketHandler::HandlePacket_C2S_PLAYERCHAT(GameSession* session, BYTE* packet, int32 packetSize)
{
	int32 chattingMsgSize;
	int32 sessionId = session->GetSessionID();
	int32 chatType;

	BufferReader br(packet);
	WCHAR text[1000] = {0};

	br.Read(chatType);
	br.Read(chattingMsgSize);
	br.ReadWString(text, chattingMsgSize);
;
	
	BYTE sendBuffer[1000];
	BufferWriter bw(sendBuffer);
	PacketHeader* pktHeader = bw.WriteReserve<PacketHeader>();

	bw.Write(chatType);
	bw.Write(sessionId);
	bw.Write(chattingMsgSize);
	bw.WriteWString(text, chattingMsgSize);

	pktHeader->_type = PacketProtocol::S2C_PLAYERCHAT;
	pktHeader->_pktSize = bw.GetWriterSize();
	
	switch (chatType)
	{
	case 0:
		MapManager::GetInstance()->BroadCast(session->GetPlayer(), sendBuffer, bw.GetWriterSize());
		break;

	case 1:
		SessionManager::GetInstance()->BroadCast(sendBuffer, bw.GetWriterSize());
		break;
	}
}

void PacketHandler::HandlePacket_C2S_PLAYERESPAWN(GameSession* session, BYTE* packet, int32 packetSize)
{
	session->GetPlayer()->ReSpawn();
}

void PacketHandler::HandlePacket_C2S_MONSTERATTACKED(GameSession* session, BYTE* packet, int32 packetSize)
{
	int32 monsterId;
	int32 damage;
	Vector3 monsterPos;

	BufferReader br(packet);
	br.Read(monsterId);
	br.Read(monsterPos);
	br.Read(damage);

	Player* attacker = session->GetPlayer();
	Vector3 attackerPos = attacker->GetPos();

	MonsterManager::GetInstnace()->AttackedMonster(monsterId, attacker, damage);
}

void PacketHandler::HandlePacket_C2S_PLAYERSTATINFO(GameSession* session, BYTE* packet, int32 packetSize)
{
	session->GetPlayer()->SendStatInfo();
}

void PacketHandler::HandlePacket_C2S_UPSTAT(GameSession* session, BYTE* packet, int32 packetSize)
{
	int32 type;
	BufferReader br(packet);
	br.Read(type);

	session->GetPlayer()->UpStat(type);
	session->GetPlayer()->SendStatInfo();
}

void PacketHandler::HandlePacket_C2S_PLAYERINIT(GameSession* session, BYTE* packet, int32 dataSize)
{
	int32 userSQ;
	int32 playerSQ;
	BufferReader br(packet);
	br.Read(userSQ);
	br.Read(playerSQ);
	session->WelcomeInitPacket(userSQ, playerSQ);
}
