#include "mybusobject.h"
#include "common.h"
#include <iostream>

using namespace std;

MyBusObject::MyBusObject(BusAttachment& bus, const char* path) : BusObject(path)
{
	QStatus status;

	/* Add the chat interface to this object */
	const InterfaceDescription* chatIntf = bus.GetInterface(intfName);
	AddInterface(*chatIntf);

	/* Store the Chat signal member away so it can be quickly looked up when signals are sent */
	chatSignalMember = chatIntf->GetMember("Chat");

	commandSendedSignalMember = chatIntf->GetMember("CommandSended");

	status =  bus.RegisterSignalHandler(this,
		static_cast<MessageReceiver::SignalHandler>(&MyBusObject::cmdSignalHandler),
		commandSendedSignalMember,
		NULL);

	if (ER_OK != status) {
		cout << "Failed to register signal handler for ChatObject::Chat " << QCC_StatusText(status) << endl;
	}
}

QStatus MyBusObject::sendClientSignal(const char* msg) 
{

	MsgArg chatArg("s", msg);

	if (0 == sessionId) {
		cout << "Sending Chat signal without a session id" << endl;
	}
	return Signal(NULL, sessionId, *chatSignalMember, &chatArg, 1, 0, 0);
}

void MyBusObject::cmdSignalHandler(const InterfaceDescription::Member* member, const char* srcPath, Message& msg)
{
	if (strcmp(msg->GetArg(0)->v_string.str,cmd) == 0)
	{
		cout << getLocalTime() << "  ";
		cout << "receive command.\n";
		receCommand = true;
	}
}
