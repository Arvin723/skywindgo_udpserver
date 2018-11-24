#pragma once

#define DEFAULTIP "0.0.0.0"
#define DEFAULTPORT 2222

class IpReader
{
private:
	char ip[30];
	int port;

	void readIp();
	void readPort();

public:
	IpReader();
	~IpReader();
	void getSeverIp(char *ip, int &port);

};
