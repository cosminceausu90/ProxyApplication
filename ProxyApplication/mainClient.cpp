#include <stdio.h>
#include <iostream>

#include "Client.h"

std::string CLIENT_IP_ADDRESS = "127.0.0.1";
#define CLIENT_PORT  (22)

int main(void)
{
	CClient client(CLIENT_IP_ADDRESS, CLIENT_PORT);
	
	if (client.Init())
	{
		client.Run();
	}

	return 0;
}