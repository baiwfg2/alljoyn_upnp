## AllJoyn即插即用
这个项目是利用AllJoyn做了一个即插即用的小测试，需求大部分是师兄提的，大致如下：

* 服务端向客户端定期发送命令，告知客户端发送描述信息
* 客户端收到命令后，就发送描述信息
* 服务收到描述信息后，存之于mongodb数据库；若有具体的数据，也可模拟向yeelink平台post数据

由于用到的外部模块较多，所以配置较麻烦，现交待如下

* 为通用起见，首先定义以下几个环境变量：
    * ALLJOYN_DIR(版本:14.02)，比如官方的SDK，根目录包括bin,inc,lib等目录
    * BOOST_DIR(版本:1.53)，根目录包括boost头文件夹和lib库目录，库需自己编译，教程可参考网络
    
	关于mongo-cxx的库不太好找，又不太大，所以就把库和头文件直接放到了项目目录mongodb中；
	tinyxml库也很小，直接放入
* 添加各模块的库目录路径和头文件目录
* 编译alljoyn程序所要求的库有
    * alljoyn.lib
ajrouter.lib
BundledRouter.obj
ws2_32.lib
crypt32.lib
iphlpapi.lib
Secur32.lib
Bcrypt.lib
Ncrypt.lib
	
    * tinyxml库
	
	* 至于mongoclient库和boost库，它有自动识别机制，所以不用添加具体文件

* 注意在工程属性的预处理器里添加符号QCC_OS_GROUP_WINDOWS、UNICODE，否则出现以下问题：
	\inc\qcc\platform.h(36): fatal error C1004: 发现意外的文件尾

	必须在属性-c/c++-所有选项-附加选项中填入：/vmm /vmg %(AdditionalOptions)，否则运行时出现错误：The value of ESP not properly saved...
	
* 由于boost和mongoclient都是动态链接，所以运行程序前要保证能找到正确的dll