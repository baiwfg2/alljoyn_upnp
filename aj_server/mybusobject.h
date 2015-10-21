#ifndef MYBUSOBJECT_H_
#define MYBUSOBJECT_H_

#include <alljoyn/BusAttachment.h>

using namespace ajn;

class MyBusObject : public BusObject
{
public:
	MyBusObject(BusAttachment& bus, const char* path);

	QStatus SendCmdSignal(const char* cmd);
	void ChatSignalHandler(const InterfaceDescription::Member* member, const char* srcPath, ajn::Message& msg);
	void Cat(const InterfaceDescription::Member* member,ajn::Message& msg);

private:
	const InterfaceDescription::Member* chatSignalMember;
	const InterfaceDescription::Member* commandSendedSignalMember;
};

#endif // !MYBUSOBJECT_H_