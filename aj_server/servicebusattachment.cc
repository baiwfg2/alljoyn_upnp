#include "servicebusattachment.h"
#include "common.h"
#include "mybuslistener.h"
#include <cstdio>

/* 没有输出信息就是最好的信息，即只在函数调用失败时打印 */

ServiceBusAttachment::ServiceBusAttachment(const char* appname,bool f):BusAttachment(appname,f)
{
		
}

void ServiceBusAttachment::advertiseService(const TransportMask mask,MyBusListener& busListener)
{
	// 发布服务三步骤
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
		chatIntf->AddMethod("cat","s",NULL,"inStr",0);//古宗海添加

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
