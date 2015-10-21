#ifndef _COMMON_H_
#define _COMMON_H_

#include <alljoyn/BusAttachment.h>
#include <signal.h>
#include <iostream>
#include "clientbusattachment.h"

extern const char* BUS_NAME;
extern const char* intfName;
extern const char* objPath;
extern ajn::SessionPort sessionPort;
extern const char* cmd;

extern std::string clientdesc;

extern bool receCommand;
extern volatile sig_atomic_t isInterruptted;
extern bool joinComplete;

extern ClientBusAttachment* clientbus;

extern ajn::SessionId sessionId;

void parseClientXml(const char* filename);
void readDescFile(const char* file);
char* getLocalTime();

#endif // !_COMMON_H_
