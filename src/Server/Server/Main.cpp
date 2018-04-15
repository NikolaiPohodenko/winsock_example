#include <iostream>
#include "TcpListener.h"

using namespace std;

void OnMessage(const TcpListener* sender, SOCKET clientSk, string msg);

void main()
{
	TcpListener server("127.0.0.1", 54010, OnMessage);
	if (server.TryInit())
		server.Run();
}

void OnMessage(const TcpListener* sender, SOCKET clientSk, string msg)
{
	sender->Send(clientSk, msg);
}