



解压下载好的文件：jsoncpp-src-0.5.0.tar.gz


利用VS2013（也可以用其他版本VS编辑器打开）打开jsoncpp-src-0.5.0\makefiles\vs71目录下的jsoncpp.sln，会出现三个Project：jsontest, lib_json, test_lib_json

检查设置：在lib_json上右击-->Properties-->Configuration Properties-->C/C++-->Code Generation,检查设置是否为RunTime  Library  为MTD　　

切换配置中需要生成的平台，win32还是x64，debug版本还是release版本



在用vs2013打开下载的JSonCpp工程文件会出现工程升级提示，直接点击确定。可以看到：


	



将jsoncpp-src-0.5.0中的头文件夹include拷贝到该工程目录下，然后建立一个lib文件夹，将上一步生成的 json_vc71_libmtd.lib放在其中。

右键修改JSonDemo的配置，注意要和上一步生成的lib对应平台



现在集成到自己项目中，首先将编译出来的json_vc71_libmtd.lib链接库和include文件夹复制到自己项目中，在项目属性中包含include文件夹，同时链接库包含json_vc71_libmtd.lib库即可



### 测试


	#include "json/json.h"
	#include <string>
	
	int main()
	{
	    std::string strValue = "{\"key1\":\"value1\"}";
	    Json::Reader reader;
	    Json::Value value;
	
	    if (reader.parse(strValue, value))
	    {
	        std::string out = value["key1"].asString();
	        std::cout << out << std::endl;
	    }
	
	    return 0;
	}





### 问题汇总

Q：

	error LNK2038: 检测到“RuntimeLibrary”的不匹配项:  值“MTd_StaticDebug”不匹配值“MDd_DynamicDebug”(database.obj 中)


A：

	后来查了下,是运行库设置的问题; 几个工程的 运行库设置不一样了:

	项目属性 -> 配置属性 -> C/C++ -> 代码生成 -> 运行库

	都设置一样就行了 多线程调试(/MTd)

	我这边使用的是MDd



下载地址:

[https://sourceforge.net/projects/jsoncpp/](https://sourceforge.net/projects/jsoncpp/)

[https://www.cnblogs.com/hpcpp/p/7889456.html](https://www.cnblogs.com/hpcpp/p/7889456.html)

[https://github.com/commshare/testHttp/tree/73b9fdb7a5b5bc94a8e5313bab3f2b629b55255b/testPOST/ZUrl](https://github.com/commshare/testHttp/tree/73b9fdb7a5b5bc94a8e5313bab3f2b629b55255b/testPOST/ZUrl)

[https://github.com/SuinegXY/WuxiaMudClient/tree/b99d65c424f95d68daeb87a01a330ca1eb42d257/Common/Util](https://github.com/SuinegXY/WuxiaMudClient/tree/b99d65c424f95d68daeb87a01a330ca1eb42d257/Common/Util)