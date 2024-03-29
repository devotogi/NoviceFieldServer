#pragma once
class GameSession;

class PacketHandler
{
public:
	static void HandlePacket(GameSession* session, BYTE* packet, int32 packetSize);

private:
	static void HandlePacket_C2S_PLAYERSYNC(GameSession* session, BYTE* packet, int32 packetSize);
	static void HandlePacket_C2S_LATENCY(GameSession* session, BYTE* packet, int32 packetSize);
	static void HandlePacket_C2S_MAPSYNC(GameSession* session, BYTE* packet, int32 packetSize);
	static void HandlePacket_C2S_PLAYERATTACK(GameSession* session, BYTE* packet, int32 packetSize);
	static void HandlePacket_C2S_PLAYERCHAT(GameSession* session, BYTE* packet, int32 packetSize);
	static void HandlePacket_C2S_PLAYERESPAWN(GameSession* session, BYTE* packet, int32 packetSize);
	static void HandlePacket_C2S_MONSTERATTACKED(GameSession* session, BYTE* packet, int32 packetSize);
	static void HandlePacket_C2S_PLAYERSTATINFO(GameSession* session, BYTE* packet, int32 packetSize);
	static void HandlePacket_C2S_UPSTAT(GameSession* session, BYTE* packet, int32 packetSize);
	static void HandlePacket_C2S_PLAYERINIT(GameSession* session, BYTE* dataPtr, int32 dataSize);
	static void HandlePacket_C2S_PLAYERSKILLSYNC(GameSession* session, BYTE* dataPtr, int32 dataSize);
};

