#include "mybuslistener.h"
#include "common.h"
#include <iostream>

using namespace std;

void MyBusListener::FoundAdvertisedName(const char* name, TransportMask transport, const char* namePrefix)
{
	// �ͻ��˵���
	/* Join the conversation */
	/* Since we are in a callback we must enable concurrent callbacks before calling a synchronous method. */
	clientbus->EnableConcurrentCallbacks();
	SessionOpts opts(SessionOpts::TRAFFIC_MESSAGES, true, SessionOpts::PROXIMITY_ANY, TRANSPORT_ANY);
	QStatus status = clientbus->JoinSession(name, sessionPort, this, sessionId, opts);
	if (ER_OK == status) {
		cout << "Joined conversation. Session id=" << sessionId << endl;

	} else {
		cout << "JoinSession failed (status=" << QCC_StatusText(status) << endl;
	}

	uint32_t timeout = 20;
	status = clientbus->SetLinkTimeout(sessionId, timeout);
	if (ER_OK != status) {
		cout << "Set link timeout failed\n";
	}
	joinComplete = true;
}

void MyBusListener::LostAdvertisedName(const char* name, TransportMask transport, const char* namePrefix)
{
	//�ͻ��˵���
	cout << "client is losing advertised name:" << name << endl;
	//s_joinComplete = false;
}

void MyBusListener::SessionLost (SessionId sessionId)
{
	// �ͻ��˵��ã��ȵ����������ٵ���LostAdvertisedName
	cout << "\nclient:session " << sessionId << " lost\n";
	joinComplete = false;
	receCommand = false;
}

void MyBusListener::NameOwnerChanged(const char* busName, const char* previousOwner, const char* newOwner)
{
	// ���Ǳ��Զ����ӵ��úü��Σ�Ϊʲô ��
	printf("NameOwnerChanged: name=%s, oldOwner=%s, newOwner=%s\n", busName, previousOwner ? previousOwner : "<none>",
		newOwner ? newOwner : "<none>");
}
