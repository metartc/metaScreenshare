//
// Copyright (c) 2019-2023 yanggaofeng
//
#ifndef SRC_YangKeyboard_H_
#define SRC_YangKeyboard_H_

#include <yangutil/sys/YangThread2.h>

class YangKeyboard:public YangThread
{
public:
	enum STATE{
		up, //!< key up
		down //!< key down
	};
	YangKeyboard(int vkey,STATE state);
	YangKeyboard();
	~YangKeyboard(void);
	yangbool m_isStart;
	yangbool m_isWin7;
	void Input();
	void stop();
	static bool Input(int vk, STATE state);
protected:
	void run();
private:
	int m_nVKey;
	STATE m_eState;
	bool IsCtrlAltDel();
	static bool SimulateCtrlAltDel();
	
};

#endif