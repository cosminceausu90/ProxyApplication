

#include <stdio.h>
#include <iostream>

#include "Server.h"

std::string SERVER_IP_ADDRESS = "127.0.0.1";	// IP Address of the server
#define SERVER_PORT  (11)	

void Server_MessageReceived(CServer* server, int client, std::string msg);

int main(void)
{
	CServer server(SERVER_IP_ADDRESS, SERVER_PORT, Server_MessageReceived);

	if (server.Init())
	{
		server.Run();
	}

	return 0;
}

void Server_MessageReceived(CServer* server, int client, std::string msg)
{
	server->Send(client, msg);
}