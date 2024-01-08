#include "pch.h"
#include "GameSession.h"
#include "Player.h"
#include "SessionManager.h"
#include "BufferWriter.h"
#include "PacketHandler.h"
#include "MapManager.h"
#include "MyDBConnection.h"
GameSession::GameSession(const SOCKET& socket, const SOCKADDR_IN& sock) : Session(socket, sock)
{
}

GameSession::~GameSession()
{
	if (_player)
		delete _player;
}

void GameSession::OnRecv(Session* session, BYTE* dataPtr, int32 dataLen)
{
	PacketHandler::HandlePacket(static_cast<GameSession*>(session), dataPtr, dataLen);
}

void GameSession::OnDisconnect()
{
	DBConnection* playerCon = PlayerDBConnectionPool::GetInstance()->Pop();
	int32 playerSQ = _player->GetPlayerSQ();
	Vector3 playerPos = _player->GetPos();
	int32 nowPort = 30002;
	int32 level = _player->GetLevel();
	int32 exp = _player->GetExp();

	// 로그아웃 시간 DB에 저장
	{
		SQLLEN len;
		SQLPrepare(playerCon->GetHSTMT(), (SQLWCHAR*)L"update player.d_player set x = ?,y = ?,z = ?,LAST_PORT = ?,EXP=?,LEVEL=? where PLAYER_SQ = ?", SQL_NTS);
		SQLBindParameter(playerCon->GetHSTMT(), 1, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_FLOAT, 0, 0, (SQLFLOAT*)&playerPos.x, 0, NULL);
		SQLBindParameter(playerCon->GetHSTMT(), 2, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_FLOAT, 0, 0, (SQLFLOAT*)&playerPos.y, 0, NULL);
		SQLBindParameter(playerCon->GetHSTMT(), 3, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_FLOAT, 0, 0, (SQLFLOAT*)&playerPos.z, 0, NULL);
		SQLBindParameter(playerCon->GetHSTMT(), 4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, (SQLINTEGER*)&nowPort, 0, NULL);
		SQLBindParameter(playerCon->GetHSTMT(), 5, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, (SQLINTEGER*)&exp, 0, NULL);
		SQLBindParameter(playerCon->GetHSTMT(), 6, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, (SQLINTEGER*)&level, 0, NULL);
		SQLBindParameter(playerCon->GetHSTMT(), 7, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, (SQLINTEGER*)&playerSQ, 0, NULL);
		SQLExecute(playerCon->GetHSTMT());
		SQLFetch(playerCon->GetHSTMT());
		SQLCloseCursor(playerCon->GetHSTMT());
	}

	SessionManager::GetInstance()->PopSession(_sessionId);

	PlayerDBConnectionPool::GetInstance()->Push(playerCon);
}

void GameSession::OnConnect()
{

}

void GameSession::WelcomeInitPacket(int32 userSQ, int32 playerSQ)	
{
	_sessionId = userSQ;
	DBConnection* con = PlayerDBConnectionPool::GetInstance()->Pop();
	
	{
		WCHAR playerName[256] = {};
		int32 level;
		int32 hp;
		int32 mp;	
		Vector3 pos;
		int32 damage;
		float speed;
		int32 defense;
		int32 playerType;
		int32 exp;

		{
			SQLPrepare(con->GetHSTMT(), (SQLWCHAR*)L"select top 1 player_name,level,hp,mp,x,y,z,damage,speed,defense,player_type,exp from player.d_player where PLAYER_SQ = ?;", SQL_NTS);
			SQLBindParameter(con->GetHSTMT(), 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, (SQLINTEGER*)&playerSQ, 0, NULL);
			SQLLEN len = 0;

			SQLBindCol(con->GetHSTMT(), 1, SQL_WCHAR, (SQLWCHAR*)&playerName, sizeof(playerName), &len);
			SQLBindCol(con->GetHSTMT(), 2, SQL_INTEGER, &level, sizeof(level), &len);
			SQLBindCol(con->GetHSTMT(), 3, SQL_INTEGER, &hp, sizeof(hp), &len);
			SQLBindCol(con->GetHSTMT(), 4, SQL_INTEGER, &mp, sizeof(mp), &len);
			SQLBindCol(con->GetHSTMT(), 5, SQL_C_FLOAT, &pos.x, sizeof(pos.x), &len);
			SQLBindCol(con->GetHSTMT(), 6, SQL_C_FLOAT, &pos.y, sizeof(pos.y), &len);
			SQLBindCol(con->GetHSTMT(), 7, SQL_C_FLOAT, &pos.z, sizeof(pos.z), &len);
			SQLBindCol(con->GetHSTMT(), 8, SQL_INTEGER, &damage, sizeof(damage), &len);
			SQLBindCol(con->GetHSTMT(), 9, SQL_C_FLOAT, &speed, sizeof(speed), &len);
			SQLBindCol(con->GetHSTMT(), 10, SQL_INTEGER, &defense, sizeof(defense), &len);
			SQLBindCol(con->GetHSTMT(), 11, SQL_INTEGER, &playerType, sizeof(playerType), &len);
			SQLBindCol(con->GetHSTMT(), 12, SQL_INTEGER, &exp, sizeof(exp), &len);
			SQLExecute(con->GetHSTMT());
			SQLFetch(con->GetHSTMT());
			SQLCloseCursor(con->GetHSTMT());
		}

		if (pos.Zero())
		{
			// 처음 접속인 경우 어디에 태어나게 할건가?
			pos = { 12,0,123 };
		}

		_player = new Player(this, userSQ, pos, playerName, level, hp, mp, damage,speed,defense, playerType, playerSQ,exp);
		SessionManager::GetInstance()->AddSession(userSQ, this);
	}
	BYTE sendBuffer[300];
	BufferWriter bw(sendBuffer);
	PacketHeader* pktHeader = bw.WriteReserve<PacketHeader>();

	int32 sessionId = _sessionId;
	int8 playerState = (int8)_player->GetState();
	int8 playerDir =   (int8)_player->GetDir();
	int8 playerMouseDir = (int8)_player->GetMouseDir();
	Vector3 playerPos = _player->GetPos();
	Quaternion playerQuaternion = _player->GetCameraLocalRotation();
	float hp = _player->GetHp();
	float mp = _player->GetMp();
	int8 level = _player->GetLevel();
	float spped = _player->GetSpeed();
	float damage = _player->GetDamage();
	WCHAR* userName = _player->GetPlayerName();
	int8 userNameSize = (int8)(wcslen(userName) * sizeof(WCHAR));
	int8 playerType = (int8)_player->GetPlayerType();
	int32 exp = _player->GetExp();
	bw.Write(sessionId);
	bw.Write(playerState);
	bw.Write(playerDir);
	bw.Write(playerMouseDir);
	bw.Write(playerPos);
	bw.Write(playerQuaternion);
	bw.Write(hp);
	bw.Write(mp);
	bw.Write(level);
	bw.Write(spped);
	bw.Write(damage);
	bw.Write(userNameSize);
	bw.WriteWString(userName, userNameSize);
	bw.Write(playerType);
	bw.Write(exp);

	pktHeader->_type = PacketProtocol::S2C_PLAYERINIT;
	pktHeader->_pktSize = bw.GetWriterSize();

	Send(sendBuffer, pktHeader->_pktSize);
	MapManager::GetInstance()->Set(_player);
	PlayerDBConnectionPool::GetInstance()->Push(con);
}
