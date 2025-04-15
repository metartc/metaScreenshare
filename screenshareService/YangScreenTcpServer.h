//
// Copyright (c) 2019-2023 yanggaofeng
//
#ifndef SRC_YangScreenTcpServer_H_
#define SRC_YangScreenTcpServer_H_
#pragma once
#include <yangutil/yangtype.h>
#include <yangutil/buffer/YangVideoBuffer.h>
#include <yangutil/sys/YangThread2.h>
class YangScreenTcpServer:public YangThread
{
public:
	YangScreenTcpServer();
	~YangScreenTcpServer();
	int32_t startListen();
	int32_t startTestListen();
	yangbool m_isLoop;
	yangbool m_isStart;
	int32_t m_connFd;
	YangVideoBuffer* m_videoBuffer;
	int32_t sendKeyMouseMessage(char* msg, int32_t nb);
	int32_t loginState();
	void stop();
protected:
	void run();
private:
	char remoteIp[32];
	YangFrame m_frame;
	int32_t m_socketfd;


};

#endif