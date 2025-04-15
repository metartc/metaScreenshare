//
// Copyright (c) 2019-2023 yanggaofeng
//
#ifndef SRC_YangScreenEvent_H_
#define SRC_YangScreenEvent_H_
#include <yangutil/yangtype.h>
#include <yangutil/yangavinfotype.h>
#include <yangcapture/YangMouse.h>
#include "YangKeyboard.h"
#include "YangMouseEvent.h"
class YangScreenEvent
{
public:
	YangScreenEvent(YangContext* context);
	~YangScreenEvent();
	void screenEvent(char* str, int32_t pnb);

private:
	char m_commandStr[256];
	YangContext* m_ini;
	YangMouseEvent* m_mouse;
	void remoteScreenWinEvent(YangScreenKeyEvent* ev);
};

#endif