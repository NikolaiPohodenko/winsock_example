#pragma once

#include <string>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")


class TcpListener;
typedef void(*OnMsgReceived)(const TcpListener* sender, SOCKET clientSk, std::string msg);


class TcpListener
{
public:
	TcpListener(std::string ip, int port, OnMsgReceived rcvHandler);
	~TcpListener();

	void Run    ();
	bool TryInit();
	void Send   (SOCKET clientSk, std::string msg) const;

private:
	std::string   m_ip;
	int           m_port;
	OnMsgReceived m_onRcv;
	size_t        m_buffSize;

	
	void   Cleanup        ();
	SOCKET CreateLsnSocket();
	SOCKET Wait4Connection(SOCKET listenSk);
	
};