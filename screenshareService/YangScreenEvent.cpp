//
// Copyright (c) 2019-2023 yanggaofeng
//
#include "YangScreenEvent.h"
#include "YangRegReader.h"
#include <yangcapture/YangMouse.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangFile.h>
#include <yangutil/sys/YangSocket.h>
#include <yangutil/sys/YangString.h>
#include <string>
#include <vector>
#include "YangKeyboard.h"
using namespace std;
yangbool yang_isWin7() {
    OSVERSIONINFO osversioninfo;

    osversioninfo.dwOSVersionInfoSize = sizeof(osversioninfo);
    GetVersionEx(&osversioninfo);

 return osversioninfo.dwMajorVersion <=6 && osversioninfo.dwMinorVersion<2;
    
}

YangScreenEvent::YangScreenEvent(YangContext* context)
{
    m_ini = context;
    m_mouse = new YangMouseEvent(m_ini);
}

YangScreenEvent::~YangScreenEvent()
{
    yang_delete(m_mouse);
}

void YangScreenEvent::screenEvent(char* str, int32_t pnb)
{

    memset(m_commandStr, 0, sizeof(m_commandStr));
    memcpy(m_commandStr, str, pnb);

    vector<string> commands = yang_split(m_commandStr, ',');
    if (commands.size() != 4)
        return;

    string type = commands[0];
    string directionStr = commands[1];
    YangScreenKeyEvent ev;
    ev.direction = directionStr;
    ev.event = type;
   
    if (type == "altctrldelkey") {
        YangKeyboard key;
        key.m_isWin7 = yang_isWin7();
        if (key.m_isWin7) return;
        if (!key.m_isWin7) {
            YangRegReader reader;
            if (reader.isDisableVad()) {
                yang_trace("\nalt+ctrl+del is disable!");
                return;
            }
       }             
       key.start();
       while (key.m_isStart) {
             yang_usleep(1000 * 20);
       }        
       return;        
    }

    if (directionStr == "key") {
        ev.key = atoi(commands[3].c_str());
        ev.x = 0;
        ev.y = 0;

    }
    else {

        int xxx = m_ini->avinfo.video.width * atof(commands[2].c_str());
        int yyy = m_ini->avinfo.video.height * atof(commands[3].c_str());
        // yang_trace("\nwidth==%d,height==%d,x=%d,y=%d", m_ini->avinfo.video.width, m_ini->avinfo.video.height,xxx,yyy);
        ev.x = xxx;
        ev.y = yyy;

    }
    if (type == "wheel") ev.wheel = atoi(directionStr.c_str());
    remoteScreenWinEvent(&ev);

}

//remote screen control
void YangScreenEvent::remoteScreenWinEvent(YangScreenKeyEvent* ev)
{

    if (ev->event == "mousePress" && ev->direction == "left")
    {
        m_mouse->moveTo(ev->x, ev->y);
        m_mouse->leftBDown();
    }
    else if (ev->event == "mousePress" && ev->direction == "right")
    {
        m_mouse->moveTo(ev->x, ev->y);
        m_mouse->rightBDown();
    }
    if (ev->event == "mouseRelease" && ev->direction == "left")
    {
        m_mouse->moveTo(ev->x, ev->y);
        m_mouse->leftBUp();
    }
    else if (ev->event == "mouseRelease" && ev->direction == "right")
    {
        m_mouse->moveTo(ev->x, ev->y);
        m_mouse->rightBUp();
    }
    if (ev->event == "mouseDouble" && ev->direction == "left")
    {
        m_mouse->moveTo(ev->x, ev->y);
        m_mouse->leftbDClick();
    }
    else if (ev->event == "mouseDouble" && ev->direction == "right")
    {
        m_mouse->moveTo(ev->x, ev->y);
        m_mouse->rightBDbClick();
    }
    else if (ev->event == "mouseMove")
    {
        m_mouse->moveTo(ev->x, ev->y);
    }
    else if (ev->event == "keyPressEvent")
    {
#ifdef _WIN32
        YangKeyboard::Input((int)ev->key, YangKeyboard::down);
        // keybd_event(ev->key, 0, 0, 0);
#else
        m_mouse->keyDown(ev->key);
#endif
    }
    else if (ev->event == "keyReleaseEvent")
    {
#ifdef _WIN32
        YangKeyboard::Input((int)ev->key, YangKeyboard::up);
        // keybd_event(ev->key, 0, KEYEVENTF_KEYUP, 0);
#else
        m_mouse->keyPress(ev->key);
#endif

    }
    else if (ev->event == "wheel") {
        m_mouse->middleBRoll(ev->x, ev->y, ev->wheel);
    }

}