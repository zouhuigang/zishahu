### 紫砂壶上链系统


### 指纹仪(中控指纹仪，型号:ZK4500)


之前做的C#的指纹项目[https://github.com/zouhuigang/fingerprint](https://github.com/zouhuigang/fingerprint)


1.安装指纹驱动软件，然后重启电脑.

https://www.zkteco.com/cn/product_detail/220.html

https://detail.1688.com/offer/44197330592.html?spm=a261b.2187593.1998088710.87.Sjb55l

https://www.eastcontrol.cn/qudongxiazai/361.html




### 摄像头


### 依赖

里面有个acmp开发程序，还有一个文档:

	链接：https://pan.baidu.com/s/1n6DBnIDr9N5SVqbytWdeFQ 
	提取码：v19s 
	复制这段内容后打开百度网盘手机App，操作更方便哦

转成mfc程序，在印象笔记国际版里面有一篇文章介绍。

https://www.evernote.com/l/AqWMXW9j6fhFWLYuBXgAwLK-OxoBZvoqVUA/


---






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



//具体到本设备：
//USB Camera有两个Pin
//Capture pin和Still pin
//Capture pin用于视频流预览
//Still pin用于响应抓拍（可以软触发和硬件触发）
//想要使用Still pin，必须先连接上Capture pin，才能正常使用Still pin


YUY2跟RGB是不一样的。
MJPG是压缩的一种编码格式，跟jpg压缩格式是不一样的。
如果你要拍照，只在在TransformFilter里抓图就行了。
要存储为BMP就直接按BMP文件的格式存储就行了，要是想存为JPG的就得用jpeblib库自己再实现压缩后再存储。
抓图参考例子：DXSDK\Samples\C++\DirectShow\Editing\StillCap


实现的效果图:


![images](./images/2.png)



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