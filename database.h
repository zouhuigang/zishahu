#pragma once
#include "db/sqlite3.h"
#include <string>
#include "table/FingerTpl.h"
using namespace std;
/*
int rc;
int i, nrows, ncols, tr;
char *errmsg = NULL;
char **results;

sqlite3 *db = NULL;
rc = sqlite3_open("zsh.db", &db);
if (rc)
{
fprintf(stderr, "can't open db!\n", sqlite3_errmsg(db));
sqlite3_close(db);
exit(1);
}
else
{
printf("db open successfully!\n");
}
sqlite3_get_table(db, "select * from clients;", &results, &nrows, &ncols, &errmsg);
printf("DB has %d rows and %d cols\n\n", nrows, ncols);
tr = (nrows + 1)*ncols;
for (i = 0; i < tr; ++i) //输出查询结果
{
printf("results[%d]= %s/n", i, results[i]); //此处可以自己定义输出格式，
}
sqlite3_free_table(results); //free
sqlite3_close(db);
*/

typedef struct {
	long id;
	char*  mobile;
	char*  fingerindex;
	char*  template_10;
}Tpl;



#pragma  warning(disable:4996)
#pragma  comment(lib,"db/sqlite3.lib")
class database
{
public:
	database();
	~database();
	void new_table();
	int AddFingerprint(string mobile, string fingerindex, string template_10, string push_time, string sign);
	Tpl*  LoadFingerprintList();//加载本地所有的指纹进入缓存
	int FingerCount;//指纹的个数
	static int ProcessInt(void* notUsed, int colCount, char** colData, char** colNames);
	FingerTpl* GetUserInfo(long autoid);
	static int ProcessOneTpl(void* notUsed, int colCount, char** colData, char** colNames);

private:
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	char** pResult;
};

