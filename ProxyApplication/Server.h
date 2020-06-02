#pragma once

#include <string>
#include <WS2tcpip.h> //win socket functions
#pragma comment(lib, "ws2_32.lib")//win socket library

#define MAX_SIZE_BUFFER (4096)

class CServer;

typedef void(*MessageServerHandler)(CServer* server, int socketId, std::string message);

class CServer 
{
public:
	CServer(std::string ipAddress, int port, MessageServerHandler handler);

	void Send(int ClientSocket, std::string msg);

	bool Init();

	void Run();

	void Cleanup();

	~CServer();

private: 

	SOCKET CreateSocket();
	SOCKET WaitForConnection(SOCKET listening);

	std::string					m_ipAddress;
	int							m_port;
	MessageServerHandler		MessageReceived;
};