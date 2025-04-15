//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGUTIL_YangMouse_
#define INCLUDE_YANGUTIL_YangMouse_
#ifdef _WIN32
#include <windows.h>
#endif
#include <stdint.h>
#include <string>
using namespace std;
struct YangScreenKeyEvent{
    uint8_t key;
    int x;
    int y;
    int wheel;
    string direction;
    string event;
};
#ifdef _WIN32
#define YangPoint POINT
#else
typedef struct{
	int32_t x;
	int32_t y;
}YangPoint;
#endif

#define Yang_Enable_Uinput 0

class YangMouse
{
public:
    YangMouse();
    virtual ~YangMouse();
#ifndef _WIN32
    static void* display;
    uint64_t root;
#endif

public:
    void    moveTo(int x, int y);
    void    relativeMove(int cx, int cy);
    void    setPos();
    void    restorePos();

    void    lockMouse();
    void    unlockMouse();

    void    leftBClick();
    void    leftbDClick();
    void    leftBDown();
    void    leftBUp();

    void    middleBClick();
    void    middleBDbClick();
    void    middleBDown();
    void    middleBUp();
    void    middleBRoll(int px,int py,int ch);


    void    rightBClick();
    void    rightBDbClick();
    void    rightBDown();
    void    rightBUp();

    void keyPress(uint8_t key);
    void keyDown(uint8_t key);
private:
    YangPoint p;


   
};
#endif
