#include "tinyxml.h"
#include <fstream>
#include <alljoyn/BusAttachment.h>
#include <string>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace std;


const char* BUS_NAME;
const char* intfName;
const char* objPath;
ajn::SessionPort sessionPort;
const char* cmd;

string clientdesc="";

bool joinComplete = false;
volatile sig_atomic_t isInterruptted = false;
bool receCommand = false;

/*
	解析客户端配置文件
*/
void parseClientXml(const char* filename)
{
	fstream file;
	file.open(filename,ios::in);
	if (!file) {
		printf("client.xml file not exist\n");
		exit(-1);
	}
	TiXmlDocument* doc = new TiXmlDocument;
	doc->LoadFile(filename);

	TiXmlElement* nameEle = doc->RootElement()->FirstChildElement()->FirstChildElement();
	BUS_NAME = nameEle->FirstChild()->Value();

	intfName = nameEle->NextSiblingElement("intfname")->FirstChild()->Value();
	objPath = nameEle->NextSiblingElement("objpath")->FirstChild()->Value();
	sessionPort = atoi(nameEle->NextSiblingElement("sessionport")->FirstChild()->Value());

	cmd = nameEle->NextSiblingElement("command")->FirstChild()->Value();

	printf("bus name:\t%s\nintfName:\t%s\nobjpath:\t%s\nsessionPort:\t%u\n",BUS_NAME,intfName,
		objPath,sessionPort);
}

void readDescFile(const char* file)
{
	string str;
	ifstream infs(file);
	if (!infs.is_open()) {
		printf("Error opening file\n");
	}

	while (infs >> str) {
		clientdesc += str;
	}
}

char* getLocalTime()
{
	time_t t = time(NULL);
	static char tmp[32];
	strftime(tmp,sizeof(tmp),"%Y/%m/%d %X",localtime(&t));
	return tmp;
}