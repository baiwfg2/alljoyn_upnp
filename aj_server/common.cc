#include "tinyxml.h"
#include "boost/date_time/posix_time/posix_time.hpp"

#include <alljoyn/BusAttachment.h>

#include <string>
#include <iostream>
#include <fstream>

#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char http_request[1024] = {0,};
char yeelink_server[] = "api.yeelink.net";
char temp_path[] = "/v1.0/device/9966/sensor/19877/datapoints";
char switch_path[] = "/v1.0/device/9966/sensor/22595/datapoints";
char apikey[] = "d3d565a59151cd21f3afdd82105e0e5a";

using namespace std;
using namespace boost::posix_time;

// ����6����������������ļ��ж���
const char* intfName;
const char* objPath;//�ݿͻ���Ҫ����һ��
const char* wellknownName;
const char* cmd;

uint32_t timeout;
ajn::SessionPort sessionPort;

ajn::SessionId sessionId;
bool joinComplete = false;	// �Ƿ����Ự�ɹ�
bool hasJoiner = false;		// �Ƿ��пͻ���

bool noShowCollection = false;	// �Ƿ����ն˲���ӡ����

volatile sig_atomic_t isInterruptted = false;

/*
	��������������ļ�
*/
void parseServiceXml(const char* filename)
{
	fstream file;
	file.open(filename,ios::in);
	if (!file) {
		cout << "service.xml file not exist" << endl;
		exit(-1);
	}
	TiXmlDocument* doc = new TiXmlDocument;
	doc->LoadFile(filename);

	TiXmlElement* nameEle = doc->RootElement()->FirstChildElement()->FirstChildElement();
	wellknownName = nameEle->FirstChild()->Value();

	intfName = nameEle->NextSiblingElement("intfname")->FirstChild()->Value();
	objPath = nameEle->NextSiblingElement("objpath")->FirstChild()->Value();
	sessionPort = atoi(nameEle->NextSiblingElement("sessionport")->FirstChild()->Value());

	cmd = nameEle->NextSiblingElement("command")->FirstChildElement()->FirstChild()->Value();
	timeout = atoi(nameEle->NextSiblingElement("command")->FirstChildElement("timeout")->FirstChild()->Value());

	printf("bus name:\t%s\nintfName:\t%s\nobjpath:\t%s\nsessionPort:\t%u\n",wellknownName,intfName,
		objPath,sessionPort);
}

string getLocalTime()
{
	ptime now = second_clock::local_time();
	return (to_iso_extended_string(now.date()) + " " + to_simple_string(now.time_of_day()));
}

time_t timetol()
{
	time_t t;
	t = time(NULL);
	return mktime(localtime(&t));
}

static void usage()
{
	printf("Usage:AJService.exe [-h] [-t <delay>]\n\n");
}

void parseCmdline(int ac,char** av)
{
	int i;
	for (i=1; i<ac; i++) {
		if (0 == strcmp("-h",av[i])) {
			noShowCollection = true;
		}
		else if (0 == strcmp("-t",av[i])) {
			++i;
			if (i == ac) {
				printf("option %s requires a parameter\n",av[i-1]);
				usage();
				exit(-1);
			}
			else {
				timeout = atoi(av[i]);
			}
		}
		else {
			printf("Unknown option:%s\n",av[i]);
		}
	}
}

void makeString(const char* tempstr)
{
	memset(http_request,0,sizeof(http_request));//ÿ������ʱ��Ҫ��http_request������

	//double value = 30;
	char http_attribute[64] = {0,};

	// Http���ݣ�������
	char http_content[64] = {0,};

	// ȷ��HTTP���ύ���� 
	sprintf( http_content , "{\"value\":%.4s}" , tempstr);

	// ȷ�� HTTP�����ײ�
	// ����POST /v1.0/device/1949/sensor/2510/datapoints HTTP/1.1\r\n
	char http_header[64] = {0,};
	sprintf( http_header , "POST %s HTTP/1.1\r\n",temp_path);
	strcpy( http_request , http_header );

	// �������� ���� Host: api.yeelink.net\r\n
	sprintf( http_attribute , "Host:%s\r\n" , yeelink_server);
	strcat( http_request , http_attribute);

	// �������� ���� U-ApiKey: ffa3826972d6cc7ba5b17e104ec59fa3
	sprintf( http_attribute , "U-ApiKey:%s\r\n" , apikey);
	strcat( http_request , http_attribute);

	// �����ύ�����ݵĳ��� ���� Content-Length:12\r\n
	sprintf( http_attribute , "Content-Length:%d\r\n" ,strlen(http_content) );
	strcat( http_request , http_attribute);

	// ���ӱ������ʽ Content-Type:application/x-www-form-urlencoded\r\n
	// �ò����ʡ��
	strcat( http_request , "Content-Type:application/x-www-form-urlencoded\r\n");
	// HTTP�ײ���HTTP���� �ָ�����
	strcat( http_request , "\r\n");

	// HTTP��������
	strcat( http_request , http_content);
}

void initWinSockAndConnect(int* psocket)
{
	/*
	WSADATA wsaData;


	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		printf("WSAStartup failed: %d\n", result);
		return;
	}*/

	int result;

	// DNS���� ���Զ��IP��ַ
	struct hostent *remote_host;
	remote_host = gethostbyname(yeelink_server);
	if( remote_host == NULL )
	{
		printf("DNS failed\r\n");
		return;
	}

	// �����׽���
	*psocket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in remote_sockaddr;
	remote_sockaddr.sin_family = AF_INET;
	remote_sockaddr.sin_port = htons(80);
	remote_sockaddr.sin_addr.s_addr = *(u_long *) remote_host->h_addr_list[0];
	memset(&(remote_sockaddr.sin_zero), 0, sizeof(remote_sockaddr.sin_zero));
	// ����Զ������
	result = connect( *psocket, (struct sockaddr *)&remote_sockaddr, sizeof(struct sockaddr));
	if( result == 0 )
	{
		//printf("connect ok\r\n");
	}
}

void sendToYeelink(const char* tempstr) //�����ַ�������
{
	int socket_id;

	makeString(tempstr);

	//printf("%s\n",http_request);

	initWinSockAndConnect(&socket_id);
	printf("Sending to Yeelink...\n");
	send(socket_id , http_request, strlen(http_request),0);

	char http_response[1024] = {0};
	int bytes_received = 0;
	bytes_received = recv( socket_id , http_response, 1024,0);
	http_response[ bytes_received ] = '\0';

	// �ж��Ƿ��յ�HTTP OK
	char* presult = strstr( http_response , "200 OK\r\n");
	if( presult == NULL ) printf("Http response error\r\n");

	//closesocket(socket_id);
}
