#include "clientbusattachment.h"
#include <iostream>
#include "common.h"
#include "mybusobject.h"

using namespace std;

ClientBusAttachment::ClientBusAttachment(const char* appname,bool f):BusAttachment(appname,f)
{

}

/** Create the interface, report the result to stdout, and return the result status. */
void ClientBusAttachment::createInterface(void)
{
    InterfaceDescription* chatIntf = NULL;
    QStatus status = CreateInterface(intfName, chatIntf);

    if (ER_OK == status) {
        chatIntf->AddSignal("Chat", "s",  "str", 0);

		chatIntf->AddSignal("CommandSended","s","cmd",0);

        chatIntf->Activate();
    } else {
        cout << "Failed to create interface\n";
    }
}

void ClientBusAttachment::startMessageBus(void)
{
    QStatus status = Start();

    if (ER_OK != status) {
        cout << "Start of BusAttachment failed\n";
    }
}

void ClientBusAttachment::registerBusObject(MyBusObject& obj)
{
    QStatus status = RegisterBusObject(obj);

    if (ER_OK != status) {
        cout << "RegisterBusObject failed\n";
    }
}

void ClientBusAttachment::connectBusAttachment(void)
{
    QStatus status = Connect();

    if (ER_OK != status) {
        cout << "Failed to connect to " << clientbus->GetConnectSpec().c_str() << endl;
    }
}

/** Begin discovery on the well-known name of the service to be called, report the result to
   stdout, and return the result status. */
void ClientBusAttachment::findAdvertisedName(void)
{
    /* Begin discovery on the well-known name of the service to be called */
    QStatus status = FindAdvertisedName(BUS_NAME);

    if (status != ER_OK) {
       printf("FindAdvertisedName ('%s') failed (%s).\n", BUS_NAME, QCC_StatusText(status));
    } 
}

/** Wait for join session to complete, report the event to stdout, and return the result status. */
QStatus ClientBusAttachment::waitForJoinSessionCompletion(void)
{
    unsigned int count = 0;

    while (!joinComplete && !isInterruptted) {
        if (0 == (count++ % 100)) {
            cout << "Waited " << count/100 << " seconds for JoinSession completion.\n";
        }

#ifdef _WIN32
        Sleep(10);
#else
        usleep(10 * 1000);
#endif
    }

    return joinComplete && !isInterruptted ? ER_OK : ER_ALLJOYN_JOINSESSION_REPLY_CONNECT_FAILED;
}