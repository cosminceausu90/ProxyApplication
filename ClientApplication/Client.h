#pragma once

#include <string>
#include <WS2tcpip.h> //win socket functions
#pragma comment(lib, "ws2_32.lib")//win socket library

#define MAX_SIZE_BUFFER (4096)

class CClient
{
public:
	CClient(std::string ipAddress, int port);
	
	bool Init();

	void Run();

	void Cleanup();

	~CClient();

private:
	SOCKET CreateSocketandConnect();
	std::string					m_ipAddress;
	int							m_port;
};

