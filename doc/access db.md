### c++ 连接本地数据库

在mfc中的stdafx.h添加:

	#ifndef _AFX_NO_DAO_SUPPORT
	#include <afxdao.h>	// MFC DAO database classes
	#endif // _AFX_NO_DAO_SUPPORT


在你需要引入的.h中添加:

	#include <afxdao.h>



q: error C4995: “CDaoDatabase”:  名称被标记为 #pragma deprecated


a：
	据说被淘汰了

### MFC访问数据库常用ADO


在mfc中的stdafx.h添加:

	#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF", "rsEOF")


### sqlite


	LIB /DEF:sqlite3.def /machine:X64

	

https://raw.githubusercontent.com/Lehuutuong/MYCODE/99c09fce6442980780578bdc4fc7f9136d427416/MapleAnalyze/_common/DbManager.cpp

https://blog.csdn.net/fuyanzhi1234/article/details/6736195


https://www.cnblogs.com/aelite/articles/6581918.html

https://www.cnblogs.com/chechen/p/7356010.html

https://www.sqlite.org/download.html

https://www.jianshu.com/p/3e4166f758c1