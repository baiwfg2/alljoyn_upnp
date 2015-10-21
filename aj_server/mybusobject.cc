#include "mongodb.h"
#include "mybusobject.h"
#include "common.h"
#include <iostream>
#include <string>

using namespace std;

MyBusObject::MyBusObject(BusAttachment& bus, const char* path) : BusObject(path)
{
	QStatus status;

	const InterfaceDescription* chatIntf = bus.GetInterface(intfName);
	AddInterface(*chatIntf);

	/* Store the Chat signal member away so it can be quickly looked up when signals are sent */
	chatSignalMember = chatIntf->GetMember("Chat");
	commandSendedSignalMember = chatIntf->GetMember("CommandSended");

	// ���ں���ӣ�Ϊ��Ӧ�ݿͻ���
	const MethodEntry methodEntries[] = {
		{ chatIntf->GetMember("cat"),static_cast<MessageReceiver::MethodHandler>(&MyBusObject::Cat) }
	};
	AddMethodHandlers(methodEntries,sizeof(methodEntries)/sizeof(methodEntries[0]));

	/* Register signal handler */
	status =  bus.RegisterSignalHandler(this,
		static_cast<MessageReceiver::SignalHandler>(&MyBusObject::ChatSignalHandler),
		chatSignalMember,
		NULL);

	if (ER_OK != status) {
		printf("Failed to register signal handler for ChatObject::Chat (%s)\n", QCC_StatusText(status));
	}
}

QStatus MyBusObject::SendCmdSignal(const char* cmd)
{
	MsgArg arg("s",cmd);

	// ֮ǰ�������������û�и�MyBusObject����ռ䣬����commandSendedSignalMember�����ڴ���쳣
	return Signal(NULL,sessionId,*commandSendedSignalMember,&arg,1,0,0);
}

void MyBusObject::ChatSignalHandler(const InterfaceDescription::Member* member, const char* srcPath, ajn::Message& msg)
{		
	static uint32_t times = 1;
	string clientdesc;
	const char* sender = msg->GetSender();

	cout << getLocalTime() << "  ";
	printf("%dth Receiving message.",times++);
	printf("\tsender:%s\n",sender);

	clientdesc = msg->GetArg(0)->v_string.str;

	// ��ȡ���¶�ֵ���͵�ƽ̨
#ifdef _SEND_TEMP
	string temp = clientdesc.substr(clientdesc.find("ASContent") + 12,5);
	printf("temp:%s\n",temp.c_str());
#endif

	clientdesc.insert(11,sender);//����joiner����Ƹ�ʽʱҪ��֤�պ�������11��λ�ò���joiner����

	printf("writing to mongodb...\n");
	writeToDb(clientdesc.c_str(),sender);
	if (!noShowCollection)
		showCollection(ns_clientdesc);//��ӡ����

	//�����¶ȵ�ƽ̨
#ifdef _SEND_TEMP
	sendToYeelink(temp.c_str());
#endif

	printf("---------------------------------------------\n\n");
}

void MyBusObject::Cat(const InterfaceDescription::Member* member,ajn::Message& msg)
{
	ChatSignalHandler(member,"",msg);
}
