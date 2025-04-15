//
// Copyright (c) 2019-2023 yanggaofeng
//


#ifndef SRC_YangScreen_H_
#define SRC_YangScreen_H_
#include <yangutil/yangtype.h>
#ifdef _MSC_VER
typedef enum {
	YangScreenWinLogon,
	YangScreenDefault
}YangScreenType;

class YangScreen
{
public:
	YangScreen();
	~YangScreen();
	static bool  selectDesktop(char* name);
	static yangbool isWinlogin();
	static bool isRdpSession(FILE* file, DWORD* ssid);

	static void toLowerCase(char* str);

	static void setStation();

	static int32_t toCurrent(bool isWinlogon);
	static int32_t toDefault();
	static int32_t toWinlogon();

	static void  logDesktop(FILE* file);
	static bool queryProcess(DWORD processId);
};
#endif
#endif
