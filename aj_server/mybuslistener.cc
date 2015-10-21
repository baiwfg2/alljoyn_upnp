#include "mongodb.h"

#include "mybuslistener.h"
#include "common.h"
#include <iostream>
#include <qcc/Debug.h>

using namespace std;
using namespace mongo;

void MyBusListener::NameOwnerChanged(const char* busName, const char* previousOwner, const char* newOwner)
{
	// ���Ǳ��Զ����ӵ��úü��Σ�Ϊʲô ��

	cout << "NameOwnerChanged: name=" << busName << ", oldOwner=" << (previousOwner ? previousOwner : "<none>") <<
		", newOwner=" << (newOwner ? newOwner : "<none>") << endl;

	if (!newOwner)
	{
		// ���豸�˳���������������
		//devicelist.erase(devicelist.find(busName),strlen(busName));
		con.remove(ns_clientdesc,QUERY("Joiner" << busName));

		// ��coll_client�ı�ʱ��ʱ������ʾ��coll_config��
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
	// ����˵���
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

	// ���豸���룬������������
	cout << getLocalTime() << "  ";
	cout << "Joiner comes,session id=" << sessionId << ".Sending command immediately...\n";
	s_obj->SendCmdSignal(cmd);
}
