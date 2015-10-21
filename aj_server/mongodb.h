#ifndef _MONGODB_H
#define _MONGODB_H

#ifdef _WIN32	//��linux�ϾͲ�������
	#ifndef __MYTEST
		#define __MYTEST	//��ֹ����ΰ���
		#include <WinSock2.h>			// ò�Ʊ��������ǰ��д
	#endif 
#endif

#include "mongo\client\dbclient.h" 

using namespace mongo;

// ͷ�ļ��в�Ҫ���������Ҫ����

extern const char* hostandport;
extern const char* ns_clientdesc;			//���ݿ��뼯�ϵ����ӣ�format is <dbname>.<collectname>[.<collectname>]*
extern const char* ns_config;

extern DBClientConnection con;

extern void parseDbXml(const char* filename);
void connectToDb(const char* hostandport);
void writeToDb(const char* msg,const char* joiner);
void showCollection(const char* collname);

#endif

