#include "udp_network.h"
#include <string>
#include <string.h>

#include <stdio.h>//for debuge

GameClient* GameClient::clientHead = NULL;
GameClient* GameClient::clientTail= NULL;

GameClient::GameClient(struct sockaddr_in cliaddr, char *usrname)
{
	memset(username, 0, 50);
	strcpy(this->username, usrname);
	//memcpy(&this->addr, &cliaddr, sizeof(cliaddr));
	this->addr = cliaddr;
	if (clientHead == NULL)
	{
		clientHead = this;
		clientTail = this;
	}
	else
	{
		clientTail->nextClient = this;
		clientTail = this;
	}
	this->nextClient = NULL;
}

void GameClient::print()
{
	GameClient * tmp = clientHead;
	printf("################\n");
	if (tmp == NULL)
	{
		printf("empty\n");
		printf("################\n");
		return ;
	}
	while (tmp)
	{
		printf("%s->", tmp->username);
		tmp = tmp->nextClient;
	}
	printf("\n################\n");
}

void GameClient::remove(char * usrname)
{
	GameClient *tmp = clientHead;
	GameClient *rmNode = NULL;

	if (tmp == NULL)
		return ;

	if (strcmp(usrname, clientHead->username) == 0)
	{
		rmNode = clientHead;
		clientHead = clientHead->nextClient;
		printf("remove head node\n");
	}
	else
	{
		while (tmp->nextClient)
		{
			if (strcmp(tmp->nextClient->username, usrname) == 0)
			{
				rmNode = tmp->nextClient;
				tmp->nextClient = tmp->nextClient->nextClient;
				rmNode->nextClient = NULL;
				printf("remove one node\n");
				break ;
			}
			tmp = tmp->nextClient;
		}
		if (tmp->nextClient == NULL)
		{
			clientTail = tmp;
			tmp = tmp->nextClient;
			clientTail->nextClient = NULL;
		}
	}

	if (rmNode != NULL)
		delete rmNode;
}

GameClient *GameClient::selectOneClient(char *usrname)
{
	GameClient *tmp = clientHead;
	while (tmp)
	{
		if (strcmp(usrname, tmp->username) == 0)
		{
			break;
		}
		else
		{
			tmp = tmp->nextClient;
		}
	}
	return tmp;
}



UdpNetWork::UdpNetWork()
{
}

UdpNetWork::UdpNetWork(char *_ip, int _port)
{
	memset(ip, 0, 30);
	strcpy(ip, _ip);
	port = _port;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(port);
	//inet_pton(AF_INET, ip, &serAddr.sin_addr.s_addr);// bug
	//int ret = inet_pton(AF_INET, "120.79.72.181", &serAddr.sin_addr.s_addr);// bug
	//printf("ret = %d\n", ret);
	printf("udpnetwork->serip:%s\n", ip);
	serAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	cliAddrLen = sizeof(struct sockaddr_in);
}

UdpNetWork::~UdpNetWork()
{}

void UdpNetWork::buildSockAndBindAddr()
{
	serSocket = socket(AF_INET, SOCK_DGRAM, 0);
	//todo:返回值判断
	if (bind(serSocket, (struct sockaddr*)&serAddr, sizeof(serAddr)) == 0)
	{
		printf("绑定成功\n");
	}
	else
	{
		printf("绑定失败\n");
	}
}

void UdpNetWork::setIp(char *_ip)
{
	strcpy(ip, _ip);
}


void UdpNetWork::setPort(int _port)
{
	port = _port;
}

int UdpNetWork::udpRecver()
{
	int retNum = 0;
	memset(recverBuf, 0, BUFFLEN);
	socklen_t socklen = sizeof(cliAddr);
	//retNum = recvfrom(serSocket, recverBuf, sizeof(recverBuf), 0, (struct sockaddr*)&cliAddr, &cliAddrLen);
	retNum = recvfrom(serSocket, recverBuf, sizeof(recverBuf), 0, (struct sockaddr*)&cliAddr, &socklen);
	recvbufLen = retNum;
	//printf("recv: %s\n", recverBuf);//for test
	
	return retNum;
}
int UdpNetWork::udpSender()
{
	int retNum = 0;
	retNum = sendto(serSocket, senderBuf, sizeof(senderBuf), 0, (struct sockaddr*)&cliAddr, cliAddrLen);
	sendbufLen = retNum;
	memset(senderBuf, 0, BUFFLEN);
	return retNum;
}
int UdpNetWork::udpSender(int msglen)
{
	int retNum = 0;
	if (msglen > sizeof(senderBuf))
		msglen = sizeof(senderBuf);
	retNum = sendto(serSocket, senderBuf, msglen, 0, (struct sockaddr*)&cliAddr, cliAddrLen);
	sendbufLen = retNum;
	memset(senderBuf, 0, BUFFLEN);
	return retNum;
}

int UdpNetWork::udpSenderToAll(char *sendmsg ,int msglen)
{
	int retNum = 0;

	GameClient *tmpcli = GameClient::clientHead;
	while (tmpcli)
	{
		//sleep(SLEEPTINE);
		retNum = sendto(serSocket, sendmsg, msglen, 0, (struct sockaddr*)&tmpcli->addr, sizeof(tmpcli->addr));
		tmpcli = tmpcli->nextClient;
	}
	return retNum;
}

void UdpNetWork::noticeOneNewLoginToAll(GameClient *curClient, char *usrname)
{
	char sendmsg[60] = {0};
	sendmsg[0] = 0x03;
	sendmsg[1] = 0x11;
	strncpy(sendmsg+2, usrname, 50);

	GameClient *tmpClient = GameClient::clientHead;
	while (tmpClient != NULL)
	{
		//sleep(SLEEPTINE);
		if (tmpClient != curClient)
			sendto(serSocket, sendmsg, sizeof(sendmsg), 0, (struct sockaddr*)&tmpClient->addr, sizeof(tmpClient->addr));
		tmpClient = tmpClient->nextClient;
	}

}

void UdpNetWork::noticeUsersToNewLogin(GameClient *curClient)
{
	GameClient * tmpClient = GameClient::clientHead;
	char sendmsg[60] = {0};
	sendmsg[0] = 0x04;
	sendmsg[1] = 0x11;
	while (tmpClient != NULL)
	{
		//sleep(SLEEPTINE);
		strncpy(sendmsg+2, tmpClient->username, 50);
		if (tmpClient != curClient)
			sendto(serSocket, sendmsg, sizeof(sendmsg), 0, (struct sockaddr*)&curClient->addr, sizeof(curClient->addr));
		tmpClient = tmpClient->nextClient;
		memset(sendmsg+2, 0, 50);
	}

}

void UdpNetWork::decodemsg()
{
	ReqType type = (ReqType)recverBuf[0];

	for (int i = 0; i < 120; i++)
	{
		printf("%02X ", (unsigned char)recverBuf[i]);
		if (i % 10 == 0)
			printf("\n");
	}
	printf("\n");


	switch (type)
	{
		case REGISTER:
		{
			int ret = -1;
			char usrNameReg[50] = {0};
			char usrPwReg[50] = {0};
			if (recverBuf[1] == 0x11)
				memcpy(usrNameReg, recverBuf+2, 50);
			if (recverBuf[52] == 0x12)
				memcpy(usrPwReg, recverBuf+53, 50);
			if (usrNameReg[49] != '\0' || usrPwReg[49] != '\0')
			{
				//todo:回消息->用户名或密码过长
				return ;
			}
			printf("用户名：%s\n", usrNameReg);// for test
			printf("密码：%s\n", usrPwReg);//for test
			//todo: 检查是否有同名
			//todo：无同名->存入库中，有同名->注册失败
			//todo：回消息
			
			//strcpy(senderBuf, "Register success!\n");
			ret = sql.addUser(usrNameReg, usrPwReg);
			
			memset(senderBuf, 0, BUFFLEN);
			senderBuf[0] = 0x01;
			senderBuf[1] = 0x21;
			senderBuf[2] = 0x01;
			senderBuf[3] = 0x22;
			senderBuf[4] = 0x00;
			if (ret != 0)
			{
				senderBuf[2] = 0x02;
				senderBuf[4] = 0x03;
			}	
			udpSender(120);
		}
		break;
		case LOGIN:
		{
			int loginRes = -1;
			bool loginsuc = false;
			char usrNameLog[50] = {0};
			char usrPwLog[50] = {0};
			if (recverBuf[1] == 0x11)
				memcpy(usrNameLog, recverBuf+2, 50);
			if (recverBuf[52] == 0x12)
				memcpy(usrPwLog, recverBuf+53, 50);
			if (usrNameLog[49] != '\0' || usrPwLog[49] != '\0')
			{
				//todo:回消息->用户名或密码过长
				return ;
			}
			printf("用户名：%s\n", usrNameLog);// for test
			printf("密码：%s\n", usrPwLog);//for test

			memset(senderBuf, 0, BUFFLEN);
			senderBuf[0] = 0x02;
			senderBuf[1] = 0x21;
			senderBuf[2] = 0x01;
			senderBuf[3] = 0x22;
			senderBuf[4] = 0x00;


			/*查询是否已登陆*/
			GameClient * selClip = GameClient::selectOneClient(usrNameLog);
			if (selClip != NULL)
			{
				printf("find the user in client list, login failed\n");
				senderBuf[2] = 0x02;
				senderBuf[4] = 0x01;
				loginsuc = false;
			}
			else
				loginRes = sql.selectUserForLogin(usrNameLog, usrPwLog);

			if (loginRes == 0)
				loginsuc = true;
			else
				loginsuc = false;

			if (loginsuc)
			{
				GameClient *tmpCli = NULL; 
				tmpCli = new GameClient(cliAddr, usrNameLog);
				printf("new a Client node\n");
				noticeOneNewLoginToAll(tmpCli, usrNameLog);
				noticeUsersToNewLogin(tmpCli);
				//cliAddr = tmpCli->addr;
			}
			else 
			{
				if (loginRes > 0)
				{
					senderBuf[2] = 0x02;
					senderBuf[4] = 0x02;
				}
			}
			cliAddrLen = sizeof(cliAddr);
			udpSender(120);
			GameClient::print();//for test
		} 
		break;

		case LOGINOUT:
		{
			if (recverBuf[1] == 0x11)
			{
				char sendmsg[60] = {0};
				char username[50] = {0};
				strncpy(username, recverBuf+2, 50);
				GameClient::remove(username);
				memcpy(sendmsg, recverBuf, 60);
				memcpy(senderBuf, recverBuf, 60);
				senderBuf[0] = 0x05;
				udpSenderToAll(sendmsg, 60);
				udpSender(60);
				GameClient::print();//for test
			}
		}
		break;

		case REFRESHPLAYERLIST:
		{
			//todo:刷新请求
			char name[50] = {0};
			if (recverBuf[1] == 0x61)
				strncpy(name, recverBuf+2, 49);
			else
				return ;

			GameClient *tmpCli = GameClient::selectOneClient(name);
			if (tmpCli == NULL)
				return ;
			noticeUsersToNewLogin(tmpCli);
		}
		break;

		case PLAYGAMEREQ:
		{
			if (recverBuf[1] == 0x61 && recverBuf[52] == 0x62
					&& recverBuf[103] == 0x63 && recverBuf[105] == 0x64
					&& recverBuf[107] == 0x65 && recverBuf[109] == 0x66)
			{
				char opName[50] = {0};
				strncpy(opName, recverBuf+53, 49);
				printf("play req to %s\n", opName);
				GameClient *opClient = GameClient::selectOneClient(opName);
				if (opClient == NULL)
				{
					//todo: 可返回申请失败给申请方
					return ;
				}
				else
				{
					cliAddr = opClient->addr;
					cliAddrLen = sizeof(opClient->addr);
					memcpy(senderBuf, recverBuf, 120);
					printf("client name is %s\n", opClient->username);
					udpSender(120);
				}
			}
			printf("$$$$$error msg\n");
		}
		break;

		case PLAYGAMERSP:
		{
			if (recverBuf[1] == 0x61 && recverBuf[52] == 0x62
					&& recverBuf[103] == 0x63 && recverBuf[105] == 0x64
					&& recverBuf[107] == 0x65 && recverBuf[109] == 0x66)
			{
				char name[50] = {0};
				char opname[50] = {0};
				strncpy(name, recverBuf+2, 49);
				strncpy(opname, recverBuf+53, 49);
				printf("play rsp to %s\n", name);
				GameClient *thisClient = GameClient::selectOneClient(name);
				GameClient *opClient = GameClient::selectOneClient(opname);
				if (thisClient == NULL || opClient == NULL)
				{
					//todo: 可返回申请失败给申请方
					return ;
				}
				else
				{
					thisClient->oppClient = opClient;
					opClient->oppClient = thisClient;
					cliAddr = thisClient->addr;
					cliAddrLen = sizeof(thisClient->addr);
					memcpy(senderBuf, recverBuf, 120);
					printf("client name is %s\n", thisClient->username);
					udpSender(120);
				}
			}
			printf("$$$$$error msg\n");
		}
		break;

		case PLACEONEPIECE:
		{
			if (recverBuf[1] == 0x61 && recverBuf[52] == 0x62
					&& recverBuf[103] == 0x19)
			{
				char name[50] = {0};
				char opname[50] = {0};
				strncpy(name, recverBuf+2, 49);
				strncpy(opname, recverBuf+53, 49);
				printf("play rsp to %s\n", name);
				//GameClient *thisClient = GameClient::selectOneClient(name);
				GameClient *opClient = GameClient::selectOneClient(opname);
				//if (thisClient == NULL || opClient == NULL)
				if (opClient == NULL)
				{
					//todo: 可返回申请失败给申请方
					return ;
				}
				else
				{
					cliAddr = opClient->addr;
					cliAddrLen = sizeof(opClient->addr);
					memcpy(senderBuf, recverBuf, 120);
					printf("PLACEONEPIECE: client name is %s\n", opClient->username);
					udpSender(120);
				}
			}
			printf("$$$$$error msg\n");
		}
		break;
		case GIVEUP:
		{
			if (recverBuf[1] == 0x61 && recverBuf[52] == 0x62)
			{
				char opname[50] = {0};
				strncpy(opname, recverBuf+53, 49);
				GameClient *opClient = GameClient::selectOneClient(opname);
				if (opClient == NULL)
				{
					return ;
				}
				else
				{
					cliAddr = opClient->addr;
					cliAddrLen = sizeof(opClient->addr);
					memcpy(senderBuf, recverBuf, 120);
					udpSender(120);
				}
			}
		}

		default:
		break;
	}
	memset(recverBuf, 0, BUFFLEN);
}

void UdpNetWork::networkTask()
{
	udpRecver();
	decodemsg();
}
