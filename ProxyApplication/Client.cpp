
#include "Client.h"
#include <iostream>


CClient::CClient(std::string ipAddress, int port) 
	: m_ipAddress(ipAddress), m_port(port)
{}

bool CClient::Init()
{
	bool winSockInit = TRUE;

	//init winsock
	WSADATA data;
	WORD version = MAKEWORD(2, 2);
	int wsResult = WSAStartup(version, &data);

	if (wsResult != 0)
	{
		std::cout << "Error Winsocket " << wsResult << std::endl;

		winSockInit = FALSE;
	}
	return winSockInit;
}

void CClient::Run()
{
	//send - receive data
	std::string clientHello;
	char buff[4096];
	std::string userInput;
	int msgRecv = 0;

	SOCKET clientSocket = CreateSocketandConnect();
	if (clientSocket == INVALID_SOCKET)
	{
		std::cout << "Error Winsocket " << WSAGetLastError() << std::endl;
	}

	bool lockWrite = TRUE;

	int bytesReceived;
	
	
	for (;;)
	{
		if (lockWrite)
		{
			userInput.clear();
			clientHello.clear();
			clientHello = "Client> ";

			std::cout << "Insert text to send to server: " << std::endl;

			getline(std::cin, userInput);
			clientHello.append(userInput);
			
			send(clientSocket, clientHello.c_str(), clientHello.size() + 1, 0);

			lockWrite = FALSE;
		}
		else
		{
			bytesReceived = recv(clientSocket, buff, 4096, 0);
			if (bytesReceived > 0)
			{
				//echo response to console
				std::cout << std::string(buff, 0, bytesReceived) << std::endl;
			}
			lockWrite = TRUE;
		}
	}
	closesocket(clientSocket);

	WSACleanup();
}

SOCKET CClient::CreateSocketandConnect()
{
	//create socket
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (clientSocket == INVALID_SOCKET)
	{
		std::cerr << "Socket invalid " << WSAGetLastError() << std::endl;
		return INVALID_SOCKET;
	}

	//hint structure to bind ip address and port
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(this->m_port);
	inet_pton(AF_INET, this->m_ipAddress.c_str(), &hint.sin_addr);

	//connect to server
	int connectServer = connect(clientSocket, (sockaddr*)&hint, sizeof(hint));

	if (connectServer == SOCKET_ERROR)
	{
		std::cerr << "Error connection to server  " << WSAGetLastError() << std::endl;
		closesocket(clientSocket);
		return SOCKET_ERROR;
	}
	return clientSocket;
}

void CClient::Cleanup()
{
	WSACleanup();
}

CClient::~CClient()
{
	Cleanup();
}