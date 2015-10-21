/*
	1、刚开始会为在c++中到底是使用cout还是printf而纠结，因为官方代码很多用了printf，如果改成cout很费力
	混用我又觉得别扭。看了网友的说法，还是不管这事吧，按习惯来。有人说cout在于简洁，printf在于效率

	2、由于BusAttachment,BusListener,BusObject之间关系交错，所以暂时未用将一个作为另外一个的成员变量
	的方法，而是采取了全部定义全局变量，然后在其它文件中引用来解决
*/

#include "mongodb.h"
#include "common.h"
#include <cstdio>

#include "mybusobject.h"
#include "mybuslistener.h"
#include "servicebusattachment.h"

// 定义全局变量
MyBusObject* s_obj = NULL;
ServiceBusAttachment* servicebus = NULL;
MyBusListener busListener;


static void SigIntHandler(int sig)
{
    isInterruptted = true;
}

void initWinSock()
{
#ifdef _WIN32
	WSAData wsaData;
	if(WSAStartup(MAKEWORD(1,1),&wsaData) != 0) {
		printf("WSAStartup failed\n");
	} 
#endif
}

// 可进一步增加命令行选项
#define	MYDEBUG
int main(int argc, char** argv)
{
#ifdef MYDEBUG
	parseServiceXml("..\\debug\\config\\service.xml");
	parseDbXml("..\\debug\\config\\mongodb.xml");//目录相对于工程文件vcproj
#else
	parseServiceXml("config\\service.xml");
	parseDbXml("config\\mongodb.xml");//目录相对于工程文件vcproj
#endif

	initWinSock();

    /* Install SIGINT handler. */
    signal(SIGINT, SigIntHandler);

	connectToDb(hostandport);
	con.dropCollection(ns_clientdesc);//每次重启将原先的集合删除
	con.dropCollection(ns_config);
	con.insert(ns_config,BSONObj());

    QStatus status = ER_OK;

	servicebus = new ServiceBusAttachment("myapp",true);//调试发现这条语句启动了一个线程，在ntdll.dll中
	servicebus->createInterface();
   	servicebus->RegisterBusListener(busListener);
    servicebus->Start();//调试发现这条语句启动了4个线程，在msvcrt110d.dll中

    MyBusObject myobj(*servicebus, objPath);
    s_obj = &myobj;

    servicebus->RegisterBusObject(*s_obj);
    servicebus->Connect();//调试发现这条语句启动了更多线程，在msvcrt110d.dll中

	const TransportMask SERVICE_TRANSPORT_TYPE = TRANSPORT_ANY;
	servicebus->advertiseService(SERVICE_TRANSPORT_TYPE,busListener);
	   
    if (ER_OK == status) {
		while (true)
		{
			if (hasJoiner) {
				printf("---------------------------------------------\n");
				
				printf("sending command...\n");
				myobj.SendCmdSignal(cmd);

#ifdef _WIN32
				Sleep(timeout);
#else
				sleep(timeout);
#endif

			}
			else {
				// 当没有客户时
				printf("waiting for joiner...\n");
				while (!hasJoiner);
			}
			
		}
		
		
    }
    return (int) status;
}

/*
//这个不能编译成功，转而用boost
char* GetLocalTime()
{
	time_t t = time(NULL);
	static char tmp[32];
	strftime(tmp,sizeof(tmp),"%Y/%m/%d %X",mongo::localtime(&t));
	return tmp;
}
*/





