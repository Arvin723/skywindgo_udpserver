#pragma once
#include <mysql/mysql.h>

class MysqlWork
{
	MYSQL mysql;
public:
	int connectToMysql();

	int addUser(char *usrname, char *usrpw);
	int selectUserForLogin(char *usrname, char *usrpw);

};
