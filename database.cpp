#include "stdafx.h"
#include "database.h"
#include <string>
#include "afx.h"
using namespace std;


//函数功能：将utf8字符转gb2312字符
//参数：    const char* utf8[IN]                   -- UTF8字符
//返回值：  char*                                  -- gb2312字符
char* U2G(const char* utf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}

//unicode字符转utf8
//函数功能：将gb2312字符转换为utf8字符
//参数：    const char* gb2312[IN]                   -- gb2312字符
//返回值：  char*                                    -- UTF8字符
char* G2U(const char* gb2312)
{
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}
database::database()
{
	//hcu
	rc = sqlite3_open("zsh.db", &db);
	
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

void database::new_table() {
	const char *sql;

	string temp;
	//userid,fingerindex,template_10,template_9,realname
	temp = "CREATE TABLE template (id INTEGER   PRIMARY KEY autoincrement NOT NULL,mobile   TEXT   NOT NULL,fingerindex  INT  NOT NULL,template_10  TEXT  NOT NULL);";
	sql = temp.c_str();
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
}
void database::AddFingerprint(string mobile, string fingerindex, string template_10) {
	const char *sql;
	string tmp;
	tmp = "insert into template(mobile, fingerindex,template_10) values('" + mobile + "'," + fingerindex + ",'" + template_10 + "');";
	sql = tmp.c_str();
	
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
}

//获取本地所有的指纹
void database::LoadFingerprintList() {
	const char *sql;
	string tmp;
	char** pResult;
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
		return;
	}

	int nIndex = nCol;
	int cnt = 0;
	for (int i = 0; i<nRow; i++)
	{
		for (int j = 0; j<nCol; j++)
		{
			
			TRACE("pData = %s,pData = %s\n", U2G(pResult[j]), U2G(pResult[nIndex]));
			++nIndex;
		}
	}

	/*CString pData[10] = { 0 };
	int nIndex = nCol;

	for (int i = 0; i < nRow; ++i)
	{
		for (int j = 0; j < nCol; ++j)
		{
		
			pData[j] = pResult[nIndex];
			
			++nIndex;
		}
	}*/

		
	sqlite3_free_table(pResult);
}

