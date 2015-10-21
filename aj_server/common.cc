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

// 下面6个变量是需从配置文件中读出
const char* intfName;
const char* objPath;//瘦客户端要保持一致
const char* wellknownName;
const char* cmd;

uint32_t timeout;
ajn::SessionPort sessionPort;

ajn::SessionId sessionId;
bool joinComplete = false;	// 是否加入会话成功
bool hasJoiner = false;		// 是否还有客户端

bool noShowCollection = false;	// 是否在终端不打印集合

volatile sig_atomic_t isInterruptted = false;

/*
	解析服务端配置文件
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
	memset(http_request,0,sizeof(http_request));//每次重组时都要把http_request给清了

	//double value = 30;
	char http_attribute[64] = {0,};

	// Http内容，表单内容
	char http_content[64] = {0,};

	// 确定HTTP表单提交内容 
	sprintf( http_content , "{\"value\":%.4s}" , tempstr);

	// 确定 HTTP请求首部
	// 例如POST /v1.0/device/1949/sensor/2510/datapoints HTTP/1.1\r\n
	char http_header[64] = {0,};
	sprintf( http_header , "POST %s HTTP/1.1\r\n",temp_path);
	strcpy( http_request , http_header );

	// 增加属性 例如 Host: api.yeelink.net\r\n
	sprintf( http_attribute , "Host:%s\r\n" , yeelink_server);
	strcat( http_request , http_attribute);

	// 增加密码 例如 U-ApiKey: ffa3826972d6cc7ba5b17e104ec59fa3
	sprintf( http_attribute , "U-ApiKey:%s\r\n" , apikey);
	strcat( http_request , http_attribute);

	// 增加提交表单内容的长度 例如 Content-Length:12\r\n
	sprintf( http_attribute , "Content-Length:%d\r\n" ,strlen(http_content) );
	strcat( http_request , http_attribute);

	// 增加表单编码格式 Content-Type:application/x-www-form-urlencoded\r\n
	// 该步骤可省略
	strcat( http_request , "Content-Type:application/x-www-form-urlencoded\r\n");
	// HTTP首部和HTTP内容 分隔部分
	strcat( http_request , "\r\n");

	// HTTP负载内容
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

	// DNS解析 获得远程IP地址
	struct hostent *remote_host;
	remote_host = gethostbyname(yeelink_server);
	if( remote_host == NULL )
	{
		printf("DNS failed\r\n");
		return;
	}

	// 创建套接字
	*psocket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in remote_sockaddr;
	remote_sockaddr.sin_family = AF_INET;
	remote_sockaddr.sin_port = htons(80);
	remote_sockaddr.sin_addr.s_addr = *(u_long *) remote_host->h_addr_list[0];
	memset(&(remote_sockaddr.sin_zero), 0, sizeof(remote_sockaddr.sin_zero));
	// 连接远程主机
	result = connect( *psocket, (struct sockaddr *)&remote_sockaddr, sizeof(struct sockaddr));
	if( result == 0 )
	{
		//printf("connect ok\r\n");
	}
}

void sendToYeelink(const char* tempstr) //暂用字符串类型
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

	// 判断是否收到HTTP OK
	char* presult = strstr( http_response , "200 OK\r\n");
	if( presult == NULL ) printf("Http response error\r\n");

	//closesocket(socket_id);
}
