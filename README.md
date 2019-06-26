zshu


![images](./images/1.png)



### ISampleGrabberCB接口


/*
定义一个类实现Sample Grabber 的回调接口IsampleGrabberCB；
调用RenderStream 依次把Still pin、Sample Grabber 和系统默认Renderer Filter 连接起来；
配置Sample Grabber 以捕获数据
*/


提供了 ISampleGrabberCB接口，ISampleGrabber接口的定义头文件 qedit.h。




### 保存图片

在win10下，win32/x64的各个版本测试通过，截图文件保存在%temp%\CaptureBmp\目录下面

	C:\Users\zouhu\AppData\Local\Temp\CaptureBmp



### 问题:

Q：

	无法打开包括文件: dxtrans.h


A：

	1.安装DirectShow SDK
	
	2.解压文件夹，编译DirectShow\Samples\C++\DirectShow\BaseClasses里的baseclasses.sln
	
	3.将对应的include加入自己的工程中
	
	若接着又出现    无法打开包括文件: dxtrans.h ：
	
	               在引用 qedit.h 头文件的时候，加上这么几句：
	
		#pragma include_alias( "dxtrans.h", "qedit.h" )
		#define __IDxtCompositor_INTERFACE_DEFINED__
		#define __IDxtAlphaSetter_INTERFACE_DEFINED__
		#define __IDxtJpeg_INTERFACE_DEFINED__
		#define __IDxtKey_INTERFACE_DEFINED__
		#include "qedit.h"


### 参考文档

[https://blog.csdn.net/longji/article/details/53455028](https://blog.csdn.net/longji/article/details/53455028)
[https://blog.csdn.net/wuxiaoyao12/article/details/7257891](https://blog.csdn.net/wuxiaoyao12/article/details/7257891)
[https://blog.csdn.net/jacken123456/article/details/80458772](https://blog.csdn.net/jacken123456/article/details/80458772)
[https://www.cnblogs.com/mlj318/p/3782534.html](https://www.cnblogs.com/mlj318/p/3782534.html)