
#include <iostream>
#include <fstream>
#include <cstring>

#include "mongodb.h"
//#include "tinystr.h"
#include "tinyxml.h"

using namespace std;

const char* ns_clientdesc;	// 2���� UGRSstatus,ASstatus
const char* ns_config;
const char* hostandport;

std::string dbname;
string tmp;

std::string coll_client;
std::string coll_config;

DBClientConnection con;

//ע��tinyxml.lib�ı��뷽ʽӦΪMDd�������̶߳�̬���Կ⣬�������MSVCRT.lib��ͻ
//#pragma comment(lib,"tinyxmld.lib")

extern time_t timetol();

/*
	�������ݿ������ļ�
*/
void parseDbXml(const char* filename)
{
	fstream file;
	file.open(filename,ios::in);
	if (!file) {
		cout << "mongodb.xml file not exist" << endl;
		exit(-1);
	}

	TiXmlDocument* doc = new TiXmlDocument;
	doc->LoadFile(filename);

	TiXmlElement* hostEle = doc->RootElement()->FirstChildElement();
	hostandport = hostEle->FirstChild()->Value();

	TiXmlElement* dbEle = hostEle->NextSiblingElement();
	dbname = dbEle->FirstAttribute()->Value();
	tmp = dbname;

	coll_client = dbEle->FirstChildElement()->FirstAttribute()->Value();
	coll_config = dbEle->FirstChildElement()->NextSiblingElement()->FirstAttribute()->Value();

	// ���dbname,coll_client����Ϊ�ֲ������������nsֵʧЧ
	ns_clientdesc = dbname.append(".").append(coll_client).c_str();
	
	ns_config = tmp.append(".").append(coll_config).c_str();
}

/*
	�������ݿ�
*/
void connectToDb(const char* host_port)
{
	try
	{
		con.connect(host_port);
		cout << "conn database.collection=" << ns_clientdesc << " ok" << endl;
	}
	catch (const DBException &e)
	{
		cout << "caught:" << e.what() << endl;
	}
}

/*
	��ʾmongodb���ݿ�ļ���
*/
void showCollection(const char* collname)
{
	auto_ptr<DBClientCursor> cursor = con.query(collname,BSONObj());
	while (cursor->more()) {
		cout << cursor->next().toString() << endl;
	}
}

/*
	д���ݵ����ݿ�
*/
void writeToDb(const char* msg,const char* joiner)
{
	BSONObj newobj = fromjson(msg);
	BSONObj obj = con.findOne(ns_clientdesc,QUERY("Joiner" << joiner));

	if (obj.isEmpty()) {
		con.insert(ns_clientdesc,newobj);
	}
	else {
		con.update(ns_clientdesc,QUERY("Joiner" << joiner),newobj);
	}
	
	//static bool configCollectionExisted = false;
	//if (!configCollectionExisted) {
		
	//	configCollectionExisted = true;
	//}
	//else
	// ��main���Ѳ����ֵ������������ֻ��Ҫ����

	BSONObj configobj = BSON("UGRSstatus" << "" << "ASstatus" << timetol());
	con.update(ns_config,Query(),configobj);

	cout << "object count:" << con.count(ns_clientdesc) << endl;
}
