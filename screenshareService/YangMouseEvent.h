//
// Copyright (c) 2019-2023 yanggaofeng
//
#ifndef SRC_YangMouseEvent_H_
#define SRC_YangMouseEvent_H_
#include <yangutil/yangtype.h>
#include <yangutil/yangavinfotype.h>

#define YangPoint POINT
class YangMouseEvent
{
public:
	YangMouseEvent(YangContext* context);
	~YangMouseEvent();
	
	int32_t mouseEvent(DWORD dw, int x, int y);
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
	void    middleBRoll(int px, int py, int ch);


	void    rightBClick();
	void    rightBDbClick();
	void    rightBDown();
	void    rightBUp();
private:
	YangContext* m_context;
	YangPoint p;

	int m_nX;
	int m_nY;
};

#endif