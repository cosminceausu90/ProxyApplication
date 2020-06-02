#include "Server.h"
#include <iostream>


CServer::CServer(std::string ipAddress, int port, MessageServerHandler handler)
	:m_ipAddress(ipAddress), m_port(port), MessageReceived(handler)
{}

void CServer::Send(int ClientSocket, std::string msg)
{
	std::string serverHello = "Server> " ;
	serverHello.append(msg);
	send(ClientSocket, serverHello.c_str(), serverHello.size() + 1, 0);
}

bool CServer::Init()
{
	bool winSockInit = TRUE;
	WSADATA data;
	WORD version = MAKEWORD(2, 2);

	int wsInit = WSAStartup(version, &data);

	if (wsInit != 0)
	{
		std::cout << "Error Winsock " << wsInit << std::endl;

		winSockInit = FALSE;
	}
	return winSockInit;
}

void CServer::Run()
{
	char buff[MAX_SIZE_BUFFER];

	for (;;)
	{
		SOCKET listening = CreateSocket();
		if (listening == INVALID_SOCKET)
		{
			break;
		}

		SOCKET client = WaitForConnection(listening);

		if (client != INVALID_SOCKET)
		{
			closesocket(listening);
			int msgRecv = 0;

			std::cout << "Client has connected" << std::endl;

			do
			{
				ZeroMemory(buff, MAX_SIZE_BUFFER);

				msgRecv = recv(client, buff, MAX_SIZE_BUFFER, 0);

				if (msgRecv > 0)
				{
					if (MessageReceived != NULL)
					{
						MessageReceived(this, client, std::string(buff, 0, msgRecv));

						std::cout << std::string(buff) << std::endl;
					}
				}

			} while (msgRecv > 0);
			
			closesocket(client);
			std::cout << "Client has disconnected" << std::endl;
		}
	}
}

SOCKET CServer::CreateSocket()
{
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);

	if (listening != INVALID_SOCKET)
	{
		sockaddr_in hint;
		hint.sin_family = AF_INET;
		hint.sin_port = htons(m_port);
		inet_pton(AF_INET, m_ipAddress.c_str(), &hint.sin_addr);

		int bindOK = bind(listening, (sockaddr*)&hint, sizeof(hint));

		if (bindOK != SOCKET_ERROR)
		{
			int listenOk = listen(listening, SOMAXCONN);

			if (listenOk == SOCKET_ERROR)
			{
				return SOCKET_ERROR;
			}
		}
		else
		{
			return SOCKET_ERROR;
		}
	}

	return listening;
}

SOCKET CServer::WaitForConnection(SOCKET listening)
{
	SOCKET client = accept(listening, NULL, NULL);

	return client;
}

void CServer::Cleanup()
{
	WSACleanup();
}

CServer::~CServer()
{
	Cleanup();
}