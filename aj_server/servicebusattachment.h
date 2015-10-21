#ifndef SERVICEBUSATTACHMENT_H_
#define SERVICEBUSATTACHMENT_H_

#include <alljoyn/BusAttachment.h>
#include "mybuslistener.h"

using namespace ajn;

class ServiceBusAttachment:public BusAttachment
{
	
public:
	ServiceBusAttachment(const char* appname,bool f);

	void advertiseService(const TransportMask mask,MyBusListener& busListener);
	
	void createInterface();
	
private:
	
	void requestName();
	void advertiseName(TransportMask mask);
	void createSession(const TransportMask mask,MyBusListener& busListener);

};

#endif
