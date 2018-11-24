#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include "mysqlwork.h"


int MysqlWork::connectToMysql()
{
	mysql_init(&mysql);
	char value = 1;
	mysql_options(&mysql, MYSQL_OPT_RECONNECT, &value);

	if (!mysql_real_connect(&mysql, "localhost", "root", "456351Go", "skywindgo", 3306, NULL, 0))
	{
		printf("connect mysql failed!\n");
		return -1;
	}
	else
	{
		printf("connect mysql seuccess\n");
		return 0;
	}

}


int MysqlWork::addUser(char *usrname, char *usrpw)
{
	char sqlstr[200] = {0};
	sprintf(sqlstr, "insert into usertb (username, password) values ('%s', '%s')", usrname, usrpw);
	printf("sql string: %s\n", sqlstr);
	int flag = mysql_real_query(&mysql, sqlstr, (unsigned int)strlen(sqlstr));
	return flag;
}

int MysqlWork::selectUserForLogin(char *usrName, char *usrpw)
{
	char sqlstr[200] = {0};
	MYSQL_RES *res = NULL;
	MYSQL_ROW row = NULL;

	sprintf(sqlstr, "select 1 from usertb where username = '%s' &&  password = '%s'", usrName, usrpw);
	printf("sql string: %s\n", sqlstr);
	int flag = mysql_real_query(&mysql, sqlstr, (unsigned int)strlen(sqlstr));
	res = mysql_store_result(&mysql);
	if (res != NULL)
	{
		my_ulonglong rows = mysql_num_rows(res);
		if(rows>0 && (row = mysql_fetch_row(res)))
		{
			printf("Login success!\n");
			return 0;
		}
		else
		{
			printf("name or password is wrong! Login failed!\n");
			return 1;
		}
	}
	else
	{
		printf("this user is not exit! Login failed!\n");
		return 1;
	}
}
