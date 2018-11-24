#include "ip_reader.h"
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <cstdlib>


IpReader::IpReader()
{
	port = -1;
	memset(ip, 0, 30);
	readIp();
	readPort();
}

IpReader::~IpReader()
{
}


void IpReader::readIp()
{
	
	//todo:各种出错检查和处理
	char ipStr[30] = {0};
	std::fstream ipFile;
	ipFile.open("server_ip.txt");
	ipFile.getline(ipStr, 30);
	strcpy(ip, ipStr);
	ipFile.close();
}


void IpReader::readPort()
{
	//todo:各种出错检查和处理
	char portStr[10] = {0};
	std::fstream portFile;
	portFile.open("server_port.txt");
	portFile.getline(portStr, 10);
	port = atoi(portStr);
	portFile.close();
}


void IpReader::getSeverIp(char * _ip, int &_port)
{
	strcpy(_ip, ip);
	_port = port;
}

