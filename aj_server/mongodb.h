#ifndef _MONGODB_H
#define _MONGODB_H

#ifdef _WIN32	//在linux上就不包含它
	#ifndef __MYTEST
		#define __MYTEST	//防止被多次包含
		#include <WinSock2.h>			// 貌似必须放在最前面写
	#endif 
#endif

#include "mongo\client\dbclient.h" 

using namespace mongo;

// 头文件中不要定义变量，要声明

extern const char* hostandport;
extern const char* ns_clientdesc;			//数据库与集合的连接，format is <dbname>.<collectname>[.<collectname>]*
extern const char* ns_config;

extern DBClientConnection con;

extern void parseDbXml(const char* filename);
void connectToDb(const char* hostandport);
void writeToDb(const char* msg,const char* joiner);
void showCollection(const char* collname);

#endif

