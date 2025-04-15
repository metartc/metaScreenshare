//
// Copyright (c) 2019-2023 yanggaofeng
//
#ifndef SRC_YangScreenCaptureWin_H_
#define SRC_YangScreenCaptureWin_H_
#pragma once
#include <yangcapture/YangScreenCapture.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangThread.h>
#include <yangutil/sys/YangSocket.h>
#include "YangScreenEvent.h"
class YangScreenCaptureWin:public YangThread
{
public:
	YangScreenCaptureWin(char* szPath);
	~YangScreenCaptureWin();
	int32_t startLoop();
	void initScreen();
	void pushVideo();
	yangbool m_isStart;
	yangbool m_isLoop;
	void stop();
	int32_t m_socket;
	YangScreenEvent* m_screenEvent;
protected:
	void run();

private:
	int m_state;
	int32_t m_pixelBits;
	int32_t m_interval;
	int64_t m_startTime;
	bool m_isDrawmouse;
	uint8_t *m_rgb,*m_yuv;
	int32_t m_bufLen;
	YangFrame m_screenFrame;
	YangContext* m_context;
	
	char m_deskName[260];
};

#endif