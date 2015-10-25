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
	// ò�Ʒǵð�2��Ctrl + C�����ܷ�Ӧ
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
		// ����̫�ؼ��ˣ�֮ǰ�����������޵ȴ�������ռ����CPU������Sleep��ʽҪ�õö�

#ifdef _WIN32
			Sleep(100);
#else
			sleep(100);
#endif
		}

		if (status == ER_OK) {
			receCommand = false;
			printf("%dth sending description...\n",times++);
			status = obj->sendClientSignal(clientdesc.c_str());//��������Ϣд��txt���������ٷ���
			printf("---------------------------------------------\n\n\n");
		}
	}

    return status;
}

// �������Ĵ��ڱ����أ�
int main(int argc, char** argv)
{
	parseClientXml("config\\client.xml");//����ڹ����ļ�Ŀ¼
	readDescFile("config\\clientdesc.txt");
	
    /* Install SIGINT handler. */
    signal(SIGINT, SigIntHandler);

	clientbus = new ClientBusAttachment("myapp",true);
	clientbus->createInterface();
	clientbus->RegisterBusListener(busListener);
	clientbus->startMessageBus();

    MyBusObject c_obj(*clientbus, objPath);
	obj = &c_obj;//ע��Ҫȷ��Ϊobj�����˿ռ�

	clientbus->registerBusObject(*obj);

    clientbus->connectBusAttachment();
   
	// ���ַ���
	clientbus->findAdvertisedName();
	clientbus->waitForJoinSessionCompletion();

    return doTheLoop();
}

/*
// ÿһ�е�\����ʡ
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





