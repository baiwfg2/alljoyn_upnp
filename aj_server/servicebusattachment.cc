#include "servicebusattachment.h"
#include "common.h"
#include "mybuslistener.h"
#include <cstdio>

/* û�������Ϣ������õ���Ϣ����ֻ�ں�������ʧ��ʱ��ӡ */

ServiceBusAttachment::ServiceBusAttachment(const char* appname,bool f):BusAttachment(appname,f)
{
		
}

void ServiceBusAttachment::advertiseService(const TransportMask mask,MyBusListener& busListener)
{
	// ��������������
	requestName();
	createSession(mask,busListener);
	advertiseName(mask);
}

void ServiceBusAttachment::createInterface()
{
	InterfaceDescription* chatIntf = NULL;
	QStatus status = CreateInterface(intfName, chatIntf);

	if (ER_OK == status) {
		chatIntf->AddSignal("Chat", "s",  "str", 0);
		chatIntf->AddMethod("cat","s",NULL,"inStr",0);//���ں����

		chatIntf->AddSignal("CommandSended","s","cmd",0);

		chatIntf->Activate();
	} else {
		printf("Failed to create interface \"%s\" (%s)\n", intfName, QCC_StatusText(status));
	}
}

void ServiceBusAttachment::requestName()
{
	QStatus status = servicebus->RequestName(wellknownName, DBUS_NAME_FLAG_DO_NOT_QUEUE);

	if (ER_OK != status) {
		printf("RequestName('%s') failed (status=%s).\n", wellknownName, QCC_StatusText(status));
	} 
}

/** Advertise the service name, report the result to stdout, and return the status code. */
void ServiceBusAttachment::advertiseName(TransportMask mask)
{
	QStatus status = servicebus->AdvertiseName(wellknownName, mask);

	if (ER_OK != status) {
		printf("Failed to advertise name '%s' (%s).\n", wellknownName, QCC_StatusText(status));
	}
}

void ServiceBusAttachment::createSession(const TransportMask mask,MyBusListener& busListener)
{
	SessionOpts opts(SessionOpts::TRAFFIC_MESSAGES, true, SessionOpts::PROXIMITY_ANY, mask);
	SessionPort sp = sessionPort;
	QStatus status = servicebus->BindSessionPort(sp, opts, busListener);

	if (ER_OK != status) {
		printf("BindSessionPort failed (%s).\n", QCC_StatusText(status));
	}
}
