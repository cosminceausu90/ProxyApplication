#include <stdio.h>
#include <iostream>
#include <string>
#include <WS2tcpip.h> //win socket functions
#pragma comment(lib, "ws2_32.lib")//win socket library

#include <thread>

#define MAX_SIZE_BUFFER (4096)

void Proxy2Server(void);
void Server2Proxy(void);

char bufFromClient[MAX_SIZE_BUFFER];
char bufFromServer[MAX_SIZE_BUFFER];


std::string SERVER_IP_ADDRESS = "127.0.0.1";	// IP Address of the server
#define SERVER_PORT  (11)						// Listening port on the server

std::string CLIENT_IP_ADDRESS = "127.0.0.1";
#define CLIENT_PORT  (22)

SOCKET serverSocket;
SOCKET clientSocket;

int main(void)
{
	std::thread worker_thread_client(Server2Proxy);
	std::thread worker_thread_server(Proxy2Server);

	worker_thread_client.join();
	worker_thread_server.join();

	return 0;
}

void Proxy2Server(void)
{
	// Init WinSocket
	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0)
	{
		std::cerr << "Winsocket err #" << wsResult << std::endl;
		return;
	}

	// Create socket
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == INVALID_SOCKET)
	{
		std::cerr << "Invalid socket err #" << WSAGetLastError() << std::endl;
		WSACleanup();
		return;
	}

	// Fill in a hint structure
	sockaddr_in hintserver;
	hintserver.sin_family = AF_INET;
	hintserver.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_IP_ADDRESS.c_str(), &hintserver.sin_addr);

	// Connect to server
	int connResult = connect(serverSocket, (sockaddr*)&hintserver, sizeof(hintserver));
	if (connResult == SOCKET_ERROR)
	{
		std::cerr << "Can't connect to server, Err #" << WSAGetLastError() << std::endl;
		closesocket(serverSocket);
		WSACleanup();
		return;
	}

	/*proxy receives from server and sends to client*/
	for (;;)
	{
		ZeroMemory(bufFromServer, MAX_SIZE_BUFFER);

		// Wait for client to send data
		int bytesReceived = recv(serverSocket, bufFromServer, MAX_SIZE_BUFFER, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			std::cerr << "Error in receive data." << std::endl;
			break;
		}

		if (bytesReceived == 0)
		{
			std::cout << "Client disconnected " << std::endl;
			break;
		}

		std::cout << std::string(bufFromServer, 0, bytesReceived) << std::endl;

		// Echo message back to client
		send(clientSocket, bufFromServer, bytesReceived, 0);
	}
}

void Server2Proxy(void)
{
	// Initialze winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		std::cerr << "Winsocket err #" << WSAGetLastError() << std::endl;
		return;
	}

	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		std::cerr << "Invalid socket err #" << WSAGetLastError() << std::endl;
		return;
	}

	// Bind the ip address and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(CLIENT_PORT);
	inet_pton(AF_INET, CLIENT_IP_ADDRESS.c_str(), &hint.sin_addr);

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell Winsock the socket is for listening 
	listen(listening, SOMAXCONN);

	// Wait for a connection
	sockaddr_in client;
	int clientSize = sizeof(client);

	clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

	char host[1024];    // client name
	char service[32];	// client port

	ZeroMemory(host, NI_MAXHOST); // same as memset(host, 0, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, 1024, service, 32, 0) == 0)
	{
		std::cout << host << " connected." << std::endl;
	}

	// Close listening socket
	closesocket(listening);

	/*proxy receives from client and sends to server*/
	for (;;)
	{
		ZeroMemory(bufFromClient, MAX_SIZE_BUFFER);

		// Wait for client to send data
		int bytesReceived = recv(clientSocket, bufFromClient, MAX_SIZE_BUFFER, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			std::cerr << "Error in receive data." << std::endl;
			break;
		}

		if (bytesReceived == 0)
		{
			std::cout << "Client disconnected " << std::endl;
			break;
		}

		std::cout << std::string(bufFromClient, 0, bytesReceived) << std::endl;

		// Echo message back to client
		send(serverSocket, bufFromClient, bytesReceived, 0);
	}

	// Close the socket
	closesocket(clientSocket);

	// Cleanup winsock
	WSACleanup();
}