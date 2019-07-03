#include "stdafx.h"
#include "database.h"
#include <string>
#include "afx.h"
#include "utils/md5.h"
using namespace std;

database::database()
{
	//hcu
	rc = sqlite3_open("zsh.db", &db);
	//_CrtSetBreakAlloc(461); //检测内存泄漏
	
}

LPCWSTR toMSG(char *szStr){
	CString str = CString(szStr);
	USES_CONVERSION;
	LPCWSTR wszClassName = A2CW(W2A(str));
	str.ReleaseBuffer();
	return wszClassName;
}

database::~database()
{
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		//::MessageBox(NULL, toMSG(zErrMsg), toMSG(zErrMsg), MB_OK);
		sqlite3_free(zErrMsg);
	}
	sqlite3_free_table(pResult);
	sqlite3_close(db);
}

string s;
int static callback(void*para, int nCount, char** pValue, char** pName) {
	s = "";
	for (int i = 0; i<nCount; i++){
		s += pValue[i];
	}
	return 0;
}

int database::ProcessInt(void* notUsed, int colCount, char** colData, char** colNames)
{
	int* id = (int*)notUsed;
	*id = atoi(colData[0]);
	return 0;
}


int database::ProcessOneTpl(void* notUsed, int colCount, char** colData, char** colNames)
{
	FingerTpl* b = (FingerTpl *)notUsed;
	//id,mobile,fingerindex,template_10,push_time,sign
	b->SetId(atoi(colData[0]));
	b->SetMobile(colData[1]);
	b->SetPushTime(colData[4]);
	b->SetSign(colData[5]);
	/*b->SetName(colData[0]);
	b->SetAnnotation(colData[3]);
	b->SetStatus(atoi(colData[2]));
	b->SetRating(atoi(colData[9]));*/

	//Author* a = new Author(atoi(colData[5]), colData[4]);
	//Publisher* p = new Publisher(atoi(colData[6]), colData[7], colData[8]);

	//b->SetAuthor(a);
	//b->SetPublisher(p);
	//TRACE("data:%d,data:%s,data:%s,data:%s \n", colData[0], colData[1], colData[2], colData[3]);
	return 0;
}


void database::new_table() {
	const char *sql;

	string temp;
	//userid,fingerindex,template_10,template_9,realname
	temp = "CREATE TABLE template (id INTEGER   PRIMARY KEY autoincrement NOT NULL,mobile   TEXT   NOT NULL,fingerindex  INT  NOT NULL,template_10  TEXT  NOT NULL,push_time TEXT  NOT NULL,sign TEXT  NOT NULL);";
	sql = temp.c_str();
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
}
int database::AddFingerprint(string mobile, string fingerindex, string template_10) {
	const char *sql;
	string tmp;

	//得到当前时间
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	char push_time[128] = { 0 };
	sprintf(push_time, "%4d-%02d-%02d %02d:%02d:%02d", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond);
	//md5
	MD5 sign;
	sign.update("zsh_" + mobile + push_time);
	//MD5(tmp1).toString();
	//TRACE("tmp1=%s,md5:%s\n", tmp1, md5.toString());

	tmp = "insert into template(mobile, fingerindex,template_10,push_time,sign) values('" + mobile + "'," + fingerindex + ",'" + template_10 + "','" + push_time + "','" + sign.toString() + "');";
	sql = tmp.c_str();

	/*rc = sqlite3_exec(db, "BEGIN;", 0, 0, &zErrMsg);
	//执行SQL语句 
	rc = sqlite3_exec(db, "COMMIT;", 0, 0, &zErrMsg);*/
	
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		sqlite3_free(zErrMsg);
		return 0;
	}

	//rc=sqlite3_exec(db, "SELECT * FROM template WHERE id=last_insert_rowid();", callback, NULL, &zErrMsg);
	int id = 0;
	rc = sqlite3_exec(db, "SELECT last_insert_rowid();", ProcessInt, &id, &zErrMsg);
	return id;

}

//获取本地所有的指纹
Tpl*  database::LoadFingerprintList() {
	const char *sql;
	string tmp;
	int nRow;
	int nCol;

	tmp = "select  * from template where 1=1;";
	sql = tmp.c_str();

	int nResult = sqlite3_get_table(db,
		sql,
		&pResult, &nRow, &nCol, &zErrMsg);
	if (nResult != SQLITE_OK)
	{
		//注意，执行失败，需要清理错误码的内存空间
		sqlite3_free(zErrMsg);
		//TRACE("error===%d\n", zErrMsg);
		return (Tpl *)NULL;
	}

	int nIndex = nCol;
	int cnt = 0;
	Tpl * tplList = new Tpl[nRow];
	for (int i = 0; i<nRow; i++)
	{
		for (int j = 0; j<nCol; j++)
		{
			//TRACE("column = %s,data = %s,nIndex=%d,nCol=%d\n", U2G(pResult[j]), U2G(pResult[nIndex]), nIndex, nCol);
			if (strcmp(pResult[j], "mobile") == 0){
				tplList[i].mobile = pResult[nIndex];
			}
			else if (strcmp(pResult[j], "fingerindex") == 0){
				tplList[i].fingerindex = pResult[nIndex];
			}
			else if (strcmp(pResult[j], "template_10") == 0){
				tplList[i].template_10 = pResult[nIndex];
			}
			else if (strcmp(pResult[j], "id") == 0){
				tplList[i].id = atol(pResult[nIndex]);
			}
			
			
			++nIndex;
		}
	}
	

	

	//释放内存
	//delete[] tplList;
	//TRACE("=====================================new mobile = %s\n", tplList[1].mobile);
	FingerCount = nRow;
	return tplList;
}

//获取个人信息
//https://github.com/Polina-Kachanova/Project-LazyCook-/blob/5a25b14ad89fb4e1bfb9c2786fe3c9e0d47bee24/sources/Database.cpp
FingerTpl* database::GetUserInfo(long autoid){
	FingerTpl *tpl = new FingerTpl();
	int rc = 0;
	const char *sql;
	
	//char* errMsg;
	//stringstream ss;
	//ss << "select b.name, b.id, b.status_book, b.annotation, a.name, a.id, p.id, p.name, p.address, b.rating from books as b join authors as a on b.author_id = a.id join publishers as p on p.id = b.publisher_id where b.id = " << id << ";";

	char tmp[128] = { 0 };
	sprintf(tmp, "select id,mobile,fingerindex,template_10,push_time,sign from template where id=%d", autoid);
	sql = tmp;
	rc = sqlite3_exec(db, sql, ProcessOneTpl, tpl, &zErrMsg);
	if (rc != SQLITE_OK) {
		sqlite3_free(zErrMsg);
	}
	return tpl;
}
