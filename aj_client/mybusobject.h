#ifndef MYBUSOBJECT_H_
#define MYBUSOBJECT_H_

#include <alljoyn/BusAttachment.h>

using namespace ajn;

class MyBusObject : public BusObject
{
public:

	MyBusObject(BusAttachment& bus, const char* path);
	QStatus sendClientSignal(const char* msg);
	void cmdSignalHandler(const InterfaceDescription::Member* member, const char* srcPath, Message& msg);

private:
	const InterfaceDescription::Member* chatSignalMember;
	const InterfaceDescription::Member* commandSendedSignalMember;
};

#endif // !MYBUSOBJECT_H_