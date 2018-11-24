#pragma once

#include <sys/types.h>
#include <sys/netinet.h>

class User
{
	int id;
	char username[50];
	char password[50];
}

class GameClient
{
	User user;
	struct sockaddr addr;
	GameClient *next_client;
}
