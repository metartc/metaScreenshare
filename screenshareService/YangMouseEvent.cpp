//
// Copyright (c) 2019-2023 yanggaofeng
//

#include "YangMouseEvent.h"
#include <yangutil/sys/YangLog.h>

inline LONG GetOpstLen(LONG len, LONG Max)
{
	if (Max == 0) return 0;
	if (len < 0)len = 0;
	if (len > Max)len = Max;
	return len * 0xFFFF / Max;
}

YangMouseEvent::YangMouseEvent(YangContext* context)
{
	m_context = context;
	m_nX = 0;
	m_nY = 0;
}

YangMouseEvent::~YangMouseEvent()
{
}
void YangMouseEvent::setPos()
{
	//GetCursorPos(&p);
}
void YangMouseEvent::moveTo(int x, int y)
{
	p.x = x;
	p.y = y;
	SetCursorPos(x, y);
	mouseEvent(MOUSEEVENTF_MOVE, x, y);
	

}
int32_t YangMouseEvent::mouseEvent(DWORD dw, int x, int y) {

	m_nX = x;
	m_nY = y;
	INPUT   input;
	ZeroMemory(&input, sizeof(INPUT));
	input.type = INPUT_MOUSE;
	
	input.mi.dx = GetOpstLen(m_nX, m_context->avinfo.video.width);
	input.mi.dy = GetOpstLen(m_nY, m_context->avinfo.video.height);
	input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE;
	input.mi.dwFlags |= dw;
	input.mi.time = 0;
	BOOL bInput = SendInput(1, &input, sizeof(INPUT));

	if (!bInput) {
		yang_error("MouseEvent SendInput failed with %d", GetLastError());
	}
	
	return Yang_Ok;
}


void YangMouseEvent::middleBRoll(int x, int y, int pch)
{
	m_nX = x;
	m_nY = y;
	INPUT input;
	memset(&input, 0, sizeof(INPUT));

	input.mi.dx = GetOpstLen(m_nX, m_context->avinfo.video.width);
	input.mi.dy = GetOpstLen(m_nY, m_context->avinfo.video.height);
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE;
	input.mi.dwFlags |= MOUSEEVENTF_WHEEL;
	input.mi.dx = x;
	input.mi.dy = y;
	input.mi.mouseData = pch;
	SendInput(1, &input, sizeof(INPUT));

}
void YangMouseEvent::leftBClick()
{
	setPos();
	mouseEvent(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, p.x, p.y);
}


void YangMouseEvent::leftbDClick()
{
	setPos();
	mouseEvent(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, p.x, p.y);
	mouseEvent(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, p.x, p.y);
}


void YangMouseEvent::leftBDown()
{
	setPos();
	mouseEvent(MOUSEEVENTF_LEFTDOWN, p.x, p.y);
}


void YangMouseEvent::leftBUp()
{
	setPos();
	mouseEvent(MOUSEEVENTF_LEFTUP, p.x, p.y);
}

//middle
void YangMouseEvent::middleBClick()
{
	setPos();
	mouseEvent(MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP, p.x, p.y);
}


void YangMouseEvent::middleBDbClick()
{
	setPos();
	mouseEvent(MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP, p.x, p.y);
	mouseEvent(MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP, p.x, p.y);
}


void YangMouseEvent::middleBDown()
{
	setPos();
	mouseEvent(MOUSEEVENTF_MIDDLEDOWN, p.x, p.y);
}


void YangMouseEvent::middleBUp()
{
	setPos();
	mouseEvent(MOUSEEVENTF_MIDDLEUP, p.x, p.y);
}




void YangMouseEvent::rightBClick()
{
	setPos();
	mouseEvent(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, p.x, p.y);
}


void YangMouseEvent::rightBDbClick()
{
	setPos();
	mouseEvent(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, p.x, p.y);
	mouseEvent(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, p.x, p.y);
}


void YangMouseEvent::rightBDown()
{
	setPos();
	mouseEvent(MOUSEEVENTF_RIGHTDOWN, p.x, p.y);
}


void YangMouseEvent::rightBUp()
{
	setPos();
	mouseEvent(MOUSEEVENTF_RIGHTUP, p.x, p.y);
}