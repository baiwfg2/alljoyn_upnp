#ifndef _COMMON_H_
#define _COMMON_H_

#include <alljoyn/BusAttachment.h>
#include <signal.h>
#include <iostream>
#include "servicebusattachment.h"
#include "mybusobject.h"

extern const char* intfName;
extern const char* objPath;//瘦客户端要保持一致
extern const char* wellknownName;
extern const char* cmd;
extern uint32_t timeout;
extern ajn::SessionPort sessionPort;

extern ajn::SessionId sessionId;
extern bool joinComplete;
extern bool hasJoiner;
extern volatile sig_atomic_t isInterruptted;
extern bool noShowCollection;

extern ServiceBusAttachment* servicebus;
extern MyBusObject* s_obj;

void parseServiceXml(const char* filename);
std::string getLocalTime();
void parseCmdline(int ac,char** av);
extern time_t timetol();

extern void sendToYeelink(const char* tempstr);

#endif // !_COMMON_H_
