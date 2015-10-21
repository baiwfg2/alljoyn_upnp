/*
	1���տ�ʼ��Ϊ��c++�е�����ʹ��cout����printf�����ᣬ��Ϊ�ٷ�����ܶ�����printf������ĳ�cout�ܷ���
	�������־��ñ�Ť���������ѵ�˵�������ǲ������°ɣ���ϰ����������˵cout���ڼ�࣬printf����Ч��

	2������BusAttachment,BusListener,BusObject֮���ϵ����������ʱδ�ý�һ����Ϊ����һ���ĳ�Ա����
	�ķ��������ǲ�ȡ��ȫ������ȫ�ֱ�����Ȼ���������ļ������������
*/

#include "mongodb.h"
#include "common.h"
#include <cstdio>

#include "mybusobject.h"
#include "mybuslistener.h"
#include "servicebusattachment.h"

// ����ȫ�ֱ���
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

// �ɽ�һ������������ѡ��
#define	MYDEBUG
int main(int argc, char** argv)
{
#ifdef MYDEBUG
	parseServiceXml("..\\debug\\config\\service.xml");
	parseDbXml("..\\debug\\config\\mongodb.xml");//Ŀ¼����ڹ����ļ�vcproj
#else
	parseServiceXml("config\\service.xml");
	parseDbXml("config\\mongodb.xml");//Ŀ¼����ڹ����ļ�vcproj
#endif

	initWinSock();

    /* Install SIGINT handler. */
    signal(SIGINT, SigIntHandler);

	connectToDb(hostandport);
	con.dropCollection(ns_clientdesc);//ÿ��������ԭ�ȵļ���ɾ��
	con.dropCollection(ns_config);
	con.insert(ns_config,BSONObj());

    QStatus status = ER_OK;

	servicebus = new ServiceBusAttachment("myapp",true);//���Է����������������һ���̣߳���ntdll.dll��
	servicebus->createInterface();
   	servicebus->RegisterBusListener(busListener);
    servicebus->Start();//���Է����������������4���̣߳���msvcrt110d.dll��

    MyBusObject myobj(*servicebus, objPath);
    s_obj = &myobj;

    servicebus->RegisterBusObject(*s_obj);
    servicebus->Connect();//���Է���������������˸����̣߳���msvcrt110d.dll��

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
				// ��û�пͻ�ʱ
				printf("waiting for joiner...\n");
				while (!hasJoiner);
			}
			
		}
		
		
    }
    return (int) status;
}

/*
//������ܱ���ɹ���ת����boost
char* GetLocalTime()
{
	time_t t = time(NULL);
	static char tmp[32];
	strftime(tmp,sizeof(tmp),"%Y/%m/%d %X",mongo::localtime(&t));
	return tmp;
}
*/





