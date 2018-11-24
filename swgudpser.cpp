#include <iostream>
#include <cstring>
#include "udp_network.h"
#include "ip_reader.h"

int main()
{
	char serverIp[30] = {0};
	int serverPort = -1;
	IpReader ipReader;
	ipReader.getSeverIp(serverIp, serverPort);
	std::cout<< "Debug--> port:"<< serverPort<<std::endl;
	std::cout<< "Debug--> ip:"<< serverIp<<std::endl;

	UdpNetWork udpNetWork(serverIp, serverPort);
	udpNetWork.buildSockAndBindAddr();
	udpNetWork.sql.connectToMysql();
	while (1)
	{
		udpNetWork.networkTask();
	}
}
