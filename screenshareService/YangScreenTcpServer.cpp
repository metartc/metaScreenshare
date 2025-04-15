
//
// Copyright (c) 2019-2023 yanggaofeng
//
#include "YangScreenTcpServer.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangTime.h>
#include <yangutil/sys/YangThread.h>
#include <yangipc/YangIpcVideoBuffer.h>
#include "yangscreen.h"
#define GetSockError()	WSAGetLastError()
#define SetSockError(e)	WSASetLastError(e)
#define setsockopt(a,b,c,d,e)	(setsockopt)(a,b,c,(const char *)d,(int)e)

#define Yang_Http_Buffer 1024*1024*10


YangScreenTcpServer::YangScreenTcpServer()
{
	m_isLoop = yangfalse;
	m_videoBuffer = yang_get_ipc_videoBuffer();
	m_isStart = yangfalse;
	m_connFd = -1;
}

YangScreenTcpServer::~YangScreenTcpServer()
{
	if (m_isStart) {
		stop();
		while (m_isStart) {
			Sleep(200);
		}
	}
}
DWORD WINAPI yang_run_tcpserver_thread(LPVOID lpParam){


	YangScreenTcpServer* server = (YangScreenTcpServer*)lpParam;

	char* buf = (char*)malloc(Yang_Http_Buffer);

	memset(buf, 0, Yang_Http_Buffer);



	char* p = NULL;

	YangFrame m_frame;
	int32_t headerLen = 0;
	int32_t nBytes = 0;
	int32_t frameLen = 0;
	server->m_isLoop = yangtrue;
	yangbool recvDataStart = yangfalse;
	int32_t recvLen = 0;
	while (server->m_isLoop) {
		memset(buf, 0, Yang_Http_Buffer);
		nBytes = recv(server->m_connFd, (char*)buf, Yang_Http_Buffer, 0);

		if (nBytes > 0) {
			
			if (recvDataStart) {
				recvLen += nBytes;
				if (recvLen >= frameLen) {
					m_frame.nb = frameLen;
					m_frame.payload = (uint8_t*)buf;
					if(server->m_videoBuffer==NULL) server->m_videoBuffer = yang_get_ipc_videoBuffer();
					if(server->m_videoBuffer){
						//m_frame.pts = m_frame.dts = yang_get_system_time() - server->m_videoBuffer->m_startTime;					
						server->m_videoBuffer->putVideo(&m_frame);					
					}
					recvDataStart = yangfalse;
					recvLen = 0;
				}
			}
			else {
				if(buf[0] == 0x02 && buf[1] == 0x00 && buf[2] == 0x00 && buf[3] == 0x02) {
					recvDataStart = yangtrue;
					if (server->m_videoBuffer == NULL) server->m_videoBuffer = yang_get_ipc_videoBuffer();
					if (server->m_videoBuffer) server->m_videoBuffer->m_loginState = yangtrue;
					recvLen = 0;
					memcpy((char*)(&frameLen), buf + 4, 4);
					//yang_trace("\nframelen==%d", frameLen);
				}
			}

			continue;

		}
		else if (nBytes == -1) {
			int32_t sockerr = GetSockError();
			if (sockerr == EINTR)
				continue;
			if (sockerr == EWOULDBLOCK || sockerr == EAGAIN || sockerr == WSAETIMEDOUT) {
				nBytes = 0;
				continue;
			}
			yang_error("%s, recv returned %d. GetSockError(): %d (%s)",
				__FUNCTION__, nBytes, sockerr, strerror(sockerr));

			break;
		}
		else if (nBytes == 0) {

			break;
		}
		break;
	}
	closesocket(server->m_connFd);
	yang_free(buf);
	return NULL;

success:
	closesocket(server->m_connFd);
	yang_free(buf);
	return Yang_Ok;
	return NULL;
}

int32_t YangScreenTcpServer::startListen()
{

	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, &wsaData);

	
	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(Yang_TcpServer_Port);

	serverAddress.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	m_socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	int32_t on = 1;

	setsockopt(m_socketfd, IPPROTO_TCP, TCP_NODELAY, (char*)&on, sizeof(on));

	int32_t timeout = 800;
	setsockopt(m_socketfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
	if (bind(m_socketfd, (struct sockaddr*)&serverAddress, sizeof(struct sockaddr_in)) < 0) {
		yang_error("http server bind error(%d)", GetSockError());
		exit(1);
	}
	listen(m_socketfd, 5);


	this->m_isLoop = 1;
	socklen_t src_len = sizeof(struct sockaddr_in);
	while (this->m_isLoop) {
		struct sockaddr_in src;
		memset(&src, 0, src_len);
		int connfd = accept(this->m_socketfd, (struct sockaddr*)&src, &src_len);
		if (connfd > -1) {
			//pthread_t th;
			m_connFd = connfd;
			memset(remoteIp, 0, sizeof(remoteIp));

			inet_ntop(AF_INET, &src.sin_addr.s_addr, remoteIp, sizeof(remoteIp));

			CreateThread(NULL, 0, yang_run_tcpserver_thread,this,0,NULL); 
			
		}
	}
	m_isStart = 0;
	closesocket(m_socketfd);
	m_socketfd = -1;
	return NULL;
}


int32_t YangScreenTcpServer::startTestListen() {
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, &wsaData);


	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(1988);

	serverAddress.sin_addr.S_un.S_addr = INADDR_ANY;

	m_socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	int32_t on = 1;

	setsockopt(m_socketfd, IPPROTO_TCP, TCP_NODELAY, (char*)&on, sizeof(on));

	int32_t timeout = 800;
	setsockopt(m_socketfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
	if (bind(m_socketfd, (struct sockaddr*)&serverAddress, sizeof(struct sockaddr_in)) < 0) {
		yang_error("http server bind error(%d)", GetSockError());
		exit(1);
	}
	listen(m_socketfd, 5);


	this->m_isLoop = 1;
	socklen_t src_len = sizeof(struct sockaddr_in);
	while (this->m_isLoop) {
		struct sockaddr_in src;
		memset(&src, 0, src_len);
		int connfd = accept(this->m_socketfd, (struct sockaddr*)&src, &src_len);
		
	}
	m_isStart = 0;
	closesocket(m_socketfd);
	m_socketfd = -1;
	return NULL;
}


int32_t YangScreenTcpServer::sendKeyMouseMessage(char* msg, int32_t nb)
{
	if(m_connFd>-1) send(m_connFd, msg, nb, 0);
	return int32_t(Yang_Ok);
}

int32_t YangScreenTcpServer::loginState()
{
	if (m_videoBuffer) return m_videoBuffer->m_loginState;
	return int32_t(0);
}

void YangScreenTcpServer::stop()
{
	m_isLoop = yangfalse;
}

void YangScreenTcpServer::run()
{
	m_isStart = yangtrue;
	startListen();
	m_isStart = yangfalse;
}


