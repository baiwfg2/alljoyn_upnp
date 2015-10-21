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

	// 古宗海添加，为适应瘦客户端
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

	// 之前曾经出现情况：没有给MyBusObject分配空间，导致commandSendedSignalMember不在内存而异常
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

	// 提取出温度值发送到平台
#ifdef _SEND_TEMP
	string temp = clientdesc.substr(clientdesc.find("ASContent") + 12,5);
	printf("temp:%s\n",temp.c_str());
#endif

	clientdesc.insert(11,sender);//插入joiner，设计格式时要保证刚好在索引11的位置插入joiner名称

	printf("writing to mongodb...\n");
	writeToDb(clientdesc.c_str(),sender);
	if (!noShowCollection)
		showCollection(ns_clientdesc);//打印集合

	//发送温度到平台
#ifdef _SEND_TEMP
	sendToYeelink(temp.c_str());
#endif

	printf("---------------------------------------------\n\n");
}

void MyBusObject::Cat(const InterfaceDescription::Member* member,ajn::Message& msg)
{
	ChatSignalHandler(member,"",msg);
}
