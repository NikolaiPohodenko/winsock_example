#include <vector>
#include "TcpListener.h"

#pragma region TcpListener members

TcpListener::TcpListener(std::string ip, int port, OnMsgReceived rcvHandler)
	: m_ip      (ip)
	, m_port    (port)
	, m_onRcv   (rcvHandler)
	, m_buffSize(4096)
{}

TcpListener::~TcpListener()
{
	Cleanup();
}

bool TcpListener::TryInit()
{
	WSAData data;
	WORD    ver = MAKEWORD(2, 2);

	int wsInit = WSAStartup(ver, &data);
	return wsInit == 0;
}

void TcpListener::Run()
{
	while (true)
	{
		SOCKET lstnSk = CreateLsnSocket();
		if (lstnSk == INVALID_SOCKET)
			break;

		SOCKET clientSk = Wait4Connection(lstnSk);
		if (clientSk == INVALID_SOCKET)
			continue;

		closesocket(lstnSk);
		
		std::vector<char> buff;
		buff.resize(m_buffSize);
		size_t bytesRecieved = 0;

		do 
		{
			ZeroMemory((char*) &(buff[0]), buff.size());
			bytesRecieved = recv(clientSk, (char*) &(buff[0]), buff.size(), 0);

			if (bytesRecieved > 0 && m_onRcv != NULL)
				m_onRcv(this, clientSk, std::string((char*) &(buff[0]), 0, bytesRecieved));

		} while (bytesRecieved > 0);
		
		closesocket(clientSk);
	}
}

void TcpListener::Send(SOCKET clientSk, std::string msg) const
{
	send(clientSk, msg.c_str(), (int) (msg.size() + 1), 0);
}

void TcpListener::Cleanup()
{
	WSACleanup(); //TODO: avoid lib deinit here. Do it elsewhere.
}

SOCKET TcpListener::CreateLsnSocket()
{
	SOCKET listenSk = socket(AF_INET /* AF_INET6 */, SOCK_STREAM /*SOCK_DGRAM */, 0);
	if (listenSk == INVALID_SOCKET)
		return INVALID_SOCKET;

	sockaddr_in hint;
	hint.sin_family = AF_INET; /* AF_INET6; */
	hint.sin_port   = htons(m_port);

	inet_pton(AF_INET /* AF_INET6 */, m_ip.c_str(), &hint.sin_addr);

	int bindRes = bind(listenSk, (sockaddr*)&hint, sizeof(hint));
	if (bindRes == SOCKET_ERROR)
		return INVALID_SOCKET;

	int lstnRes = listen(listenSk, SOMAXCONN);
	if (lstnRes == SOCKET_ERROR)
		return INVALID_SOCKET;

	return listenSk;
}

SOCKET TcpListener::Wait4Connection(SOCKET listenSk)
{
	SOCKET clientSk = accept(listenSk, NULL, NULL);
	return clientSk;
}
#pragma endregion

