//
// Copyright (c) 2019-2023 yanggaofeng
//
#include <yangcapture/YangMouse.h>
#include <yangutil/sys/YangLog.h>
#ifndef __ANDROID__
#ifndef _WIN32


#define  YANG_LEFT_MOUSE_BUTTON   1
#define  YANG_MIDDLE_MOUSE_BUTTON 2
#define  YANG_RIGHT_MOUSE_BUTTON  3


#endif


YangMouse::YangMouse() {

#ifndef _WIN32
	root = -1;
#if Yang_Enable_Uinput
	if(g_yang_mouse==NULL)
		g_yang_mouse=new YangLinuxMouse();
#endif

#endif

}



YangMouse::~YangMouse() {

}
#ifdef _WIN32

void YangMouse::moveTo(int x, int y)
{
    p.x = x;
    p.y = y;
    SetCursorPos(x, y);
}


void YangMouse::relativeMove(int cx, int cy)
{
    GetCursorPos(&p);
    p.x += cx;
    p.y += cy;
    SetCursorPos(p.x, p.y);
}


void YangMouse::setPos()
{
    GetCursorPos(&p);
}


void YangMouse::restorePos()
{
    SetCursorPos(p.x, p.y);
}


void YangMouse::lockMouse()
{
    POINT    pt;
    RECT    rt;

    GetCursorPos(&pt);
    rt.left = rt.right = pt.x;
    rt.top = rt.bottom = pt.y;
    rt.right++;
    rt.bottom++;
    ClipCursor(&rt);
}


void YangMouse::unlockMouse()
{
    ClipCursor(NULL);
}


void YangMouse::leftBClick()
{
    setPos();
    mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, p.x, p.y, 0, 0);
}


void YangMouse::leftbDClick()
{
    setPos();
    mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, p.x, p.y, 0, 0);
    mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, p.x, p.y, 0, 0);
}


void YangMouse::leftBDown()
{
    setPos();
    mouse_event(MOUSEEVENTF_LEFTDOWN, p.x, p.y, 0, 0);
}


void YangMouse::leftBUp()
{
    setPos();
    mouse_event(MOUSEEVENTF_LEFTUP, p.x, p.y, 0, 0);
}

//middle
void YangMouse::middleBClick()
{
    setPos();
    mouse_event(MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP, p.x, p.y, 0, 0);
}


void YangMouse::middleBDbClick()
{
    setPos();
    mouse_event(MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP, p.x, p.y, 0, 0);
    mouse_event(MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP, p.x, p.y, 0, 0);
}


void YangMouse::middleBDown()
{
    setPos();
    mouse_event(MOUSEEVENTF_MIDDLEDOWN, p.x, p.y, 0, 0);
}


void YangMouse::middleBUp()
{
    setPos();
    mouse_event(MOUSEEVENTF_MIDDLEUP, p.x, p.y, 0, 0);
}


void YangMouse::middleBRoll(int x,int y,int ch)
{
    setPos();
    mouse_event(MOUSEEVENTF_WHEEL, x, y, ch, 0);
}

void YangMouse::rightBClick()
{
    setPos();
    mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, p.x, p.y, 0, 0);
}


void YangMouse::rightBDbClick()
{
    setPos();
    mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, p.x, p.y, 0, 0);
    mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, p.x, p.y, 0, 0);
}


void YangMouse::rightBDown()
{
    setPos();
    mouse_event(MOUSEEVENTF_RIGHTDOWN, p.x, p.y, 0, 0);
}


void YangMouse::rightBUp()
{
    setPos();
    mouse_event(MOUSEEVENTF_RIGHTUP, p.x, p.y, 0, 0);
}
#endif
#endif
