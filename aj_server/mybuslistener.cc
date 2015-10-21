#include "mongodb.h"

#include "mybuslistener.h"
#include "common.h"
#include <iostream>
#include <qcc/Debug.h>

using namespace std;
using namespace mongo;

void MyBusListener::NameOwnerChanged(const char* busName, const char* previousOwner, const char* newOwner)
{
	// 总是被自动连接调用好几次，为什么 ？

	cout << "NameOwnerChanged: name=" << busName << ", oldOwner=" << (previousOwner ? previousOwner : "<none>") <<
		", newOwner=" << (newOwner ? newOwner : "<none>") << endl;

	if (!newOwner)
	{
		// 有设备退出，立即发送命令
		//devicelist.erase(devicelist.find(busName),strlen(busName));
		con.remove(ns_clientdesc,QUERY("Joiner" << busName));

		// 当coll_client改变时，时间标记显示在coll_config中
		BSONObj configobj = BSON("UGRSstatus" << "" << "ASstatus" << timetol());
		con.update(ns_config,Query(),configobj);

		if (con.findOne(ns_clientdesc,Query()).isEmpty())
		{
			cout << "\n\tNo client.\n\n";
			hasJoiner = false;
		}
		else
		{
			cout << getLocalTime() << "  ";
			cout << "Joiner exited.Sending Command immediately\n\n";
			s_obj->SendCmdSignal(cmd);
		}	
	}
}

bool MyBusListener::AcceptSessionJoiner(SessionPort sessionPort, const char* joiner, const SessionOpts& opts)
{
	// 服务端调用
	if (sessionPort != sessionPort) {
		cout << "Rejecting join attempt on wrong session port";
		return false;
	}

	cout << "\nAccepting join session request from " << joiner << endl;
	return true;
}

void MyBusListener::SessionJoined(SessionPort sessionPort, SessionId id, const char* joiner)
{
	hasJoiner = true;
	sessionId = id;

	servicebus->EnableConcurrentCallbacks();
	uint32_t timeout = 20;
	QStatus status = servicebus->SetLinkTimeout(sessionId, timeout);
	if (ER_OK != status) {
		QCC_SyncPrintf("Set link timeout failed\n");
	} 

	// 有设备进入，立即发送命令
	cout << getLocalTime() << "  ";
	cout << "Joiner comes,session id=" << sessionId << ".Sending command immediately...\n";
	s_obj->SendCmdSignal(cmd);
}
