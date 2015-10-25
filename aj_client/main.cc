#ifndef _WIN32
#define QCC_OS_GROUP_POSIX
#endif

#include <cstdlib>
#include <ctime>
#include <iostream>

#include <signal.h>
#include "clientbusattachment.h"
#include "common.h"
#include "mybuslistener.h"
#include "mybusobject.h"

using namespace std;

ClientBusAttachment* clientbus = NULL;

MyBusObject* obj = NULL;
MyBusListener busListener;

ajn::SessionId sessionId;

static void SigIntHandler(int sig)
{
	// 貌似非得按2次Ctrl + C，才能反应
    isInterruptted = true;
}

QStatus doTheLoop() 
{
	uint32_t times = 1;
    QStatus status = ER_OK;

	while (!joinComplete);
	while (joinComplete)
	{
		printf("---------------------------------------------\n");
		printf("waiting to receive service's command...\n");
		while (!receCommand) {
		// 这里太关键了，之前的做法是无限等待，严重占用了CPU，换成Sleep方式要好得多

#ifdef _WIN32
			Sleep(100);
#else
			sleep(100);
#endif
		}

		if (status == ER_OK) {
			receCommand = false;
			printf("%dth sending description...\n",times++);
			status = obj->sendClientSignal(clientdesc.c_str());//将描述信息写入txt，读出来再发送
			printf("---------------------------------------------\n\n\n");
		}
	}

    return status;
}

// 怎样更改窗口标题呢？
int main(int argc, char** argv)
{
	parseClientXml("config\\client.xml");//相对于工程文件目录
	readDescFile("config\\clientdesc.txt");
	
    /* Install SIGINT handler. */
    signal(SIGINT, SigIntHandler);

	clientbus = new ClientBusAttachment("myapp",true);
	clientbus->createInterface();
	clientbus->RegisterBusListener(busListener);
	clientbus->startMessageBus();

    MyBusObject c_obj(*clientbus, objPath);
	obj = &c_obj;//注意要确保为obj分配了空间

	clientbus->registerBusObject(*obj);

    clientbus->connectBusAttachment();
   
	// 发现服务
	clientbus->findAdvertisedName();
	clientbus->waitForJoinSessionCompletion();

    return doTheLoop();
}

/*
// 每一行的\不能省
const char* joinerDescription = "Joiner:\"\", \
ASName: \"\", \
ASDescription: \"\", \
ASType: \"value\", \
ASURL: \"\", \
ASMethod: \"\",\
ASContent: \"\",\
ASRequestHeader: \"\", \
ASResponseUnit: \"\"}"; 
*/





