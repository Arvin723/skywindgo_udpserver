#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "mysqlwork.h"

#define BUFFLEN 500
#define SLEEPTINE 200

enum ReqType
{
	UNKNOWREQ = 0x00,

	REGISTER = 0x01,
	LOGIN = 0x02,
	LOGINOUT = 0x05,
	REFRESHPLAYERLIST = 0x06,
	PLAYGAMEREQ  = 0x69,
	PLAYGAMERSP = 0x70,
	PLACEONEPIECE = 0x71,
	GIVEUP = 0x72

};


class GameClient
{
private:
	
public:
	GameClient(struct sockaddr_in cliaddr, char *usrname);


	int id;
	char username[50];
	struct sockaddr_in addr;
	static GameClient *clientHead;
	static GameClient *clientTail;
	GameClient *oppClient;
	GameClient *nextClient;

	static void remove(char *usrname);
	static void print();
	static GameClient * selectOneClient(char *usrname);
	
};


class UdpNetWork
{
public:
	UdpNetWork();
	UdpNetWork(char *_ip, int _port);
	~UdpNetWork();

	MysqlWork sql;
	char ip[30];
	int port;
	int serSocket;
	char senderBuf[BUFFLEN];
	int sendbufLen;
	char recverBuf[BUFFLEN];
	int recvbufLen;

	struct sockaddr_in serAddr;
	struct sockaddr_in cliAddr;
	socklen_t cliAddrLen;
	
	void buildSockAndBindAddr();
	void setIp(char *_ip);
	void setPort(int _port);
	
	int udpRecver();
	
	int udpSender();
	int udpSender(int msglen);
	int udpSenderToAll(char *sendmsg, int msglen);
	void noticeOneNewLoginToAll(GameClient *curClient, char *usrname);
	void noticeUsersToNewLogin(GameClient *curClient);
	
	void decodemsg();
	void networkTask();


	//todo:client set.....
	//todo:recvfrom    sendto.........
};
