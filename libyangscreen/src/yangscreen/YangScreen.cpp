#include <yangscreen/YangScreen.h>
#ifdef _MSC_VER
#include <windows.h>
#include "wtsapi32.h"
#include <yangutil/sys/YangLog.h>
#include <string>
#include <stdio.h>

bool selectHDESK(HDESK new_desktop)
{	
	HDESK old_desktop = GetThreadDesktop(GetCurrentThreadId());

	if (old_desktop == NULL)
		yang_error(("GetThreadDesktop with %d"), GetLastError());
	DWORD dummy;
	char new_name[256];

	if (!GetUserObjectInformation(new_desktop, UOI_NAME, new_name, 256, &dummy)) {
		yang_error(("GetUserObjectInformation with %d"), GetLastError());
		return FALSE;
	}

	// Switch the desktop
	if (!SetThreadDesktop(new_desktop)) {
		yang_error(("SetThreadDesktop with %d"), GetLastError());
		return FALSE;
	}

	// Switched successfully - destroy the old desktop
	CloseDesktop(old_desktop);

	return TRUE;
}


YangScreen::YangScreen()
{
}

YangScreen::~YangScreen()
{
}
/**
int32_t YangScreen::selectDesktop(YangScreenType desktype)
{
    HDESK desktop=NULL;
    if (desktype == YangScreenWinLogon) {
        desktop = OpenDesktop("Winlogon", 0, TRUE,
			DESKTOP_CREATEMENU |
			DESKTOP_CREATEWINDOW |
			DESKTOP_ENUMERATE |
			DESKTOP_HOOKCONTROL |
			DESKTOP_WRITEOBJECTS |
			DESKTOP_READOBJECTS |
			DESKTOP_SWITCHDESKTOP |
			GENERIC_WRITE);
	}
	else if (desktype == YangScreenDefault) {
		desktop = OpenDesktop("Default", 0, TRUE,
			DESKTOP_CREATEMENU |
			DESKTOP_CREATEWINDOW |
			DESKTOP_ENUMERATE |
			DESKTOP_HOOKCONTROL |
			DESKTOP_WRITEOBJECTS |
			DESKTOP_READOBJECTS |
			DESKTOP_SWITCHDESKTOP |
			GENERIC_WRITE);
	}
   
    

    bool result = SetThreadDesktop(desktop) != 0;
	CloseDesktop(desktop);
    return int32_t(0);
}

**/

bool YangScreen::selectDesktop(char* name)
{
	// Are we running on NT?

	HDESK desktop;
	if (name != NULL)
	{

		// Attempt to open the named desktop
		desktop = OpenDesktop(name, 0, FALSE,
			DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW |
			DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL |
			DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS |
			DESKTOP_SWITCHDESKTOP | GENERIC_WRITE);
	}
	else
	{

		// No, so open the input desktop
		desktop = OpenInputDesktop(0, FALSE,
			DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW |
			DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL |
			DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS |
			DESKTOP_SWITCHDESKTOP | GENERIC_WRITE);
	}

	// Did we succeed?
	if (desktop == NULL) {
		yang_error(("OpenInputDesktop/OpenDesktop failed with %d"), GetLastError());
		DWORD err = GetLastError();
		return FALSE;
	}

	// Switch to the new desktop
	if (!selectHDESK(desktop)) {
		// Failed to enter the new desktop, so free it!
		CloseDesktop(desktop);

		return FALSE;
	}

	// We successfully switched desktops!
	return TRUE;


	//return (name == NULL);
}

void  YangScreen::logDesktop(FILE* file) {
	HDESK inputDesktop = OpenInputDesktop(0, TRUE,
		DESKTOP_CREATEMENU |
		DESKTOP_CREATEWINDOW |
		DESKTOP_ENUMERATE |
		DESKTOP_HOOKCONTROL |
		DESKTOP_WRITEOBJECTS |
		DESKTOP_READOBJECTS |
		DESKTOP_SWITCHDESKTOP |
		GENERIC_WRITE);
	char m_deskName[260];
	memset(m_deskName, 0, sizeof(m_deskName));
	char str[256] = { 0 };
	if (GetUserObjectInformation(inputDesktop, UOI_NAME, m_deskName, sizeof(m_deskName), 0)) {
		memset(str, 0, sizeof(str));
		sprintf(str, "\ndesktop name==%s", m_deskName);
		if (file) {
			fwrite(str, strlen(str), 1, file);
			fflush(file);
		}
	}
	CloseDesktop(inputDesktop);
	inputDesktop = NULL;
}

yangbool YangScreen::isWinlogin()
{
	HDESK inputDesktop = OpenInputDesktop(0, TRUE,
		DESKTOP_CREATEMENU |
		DESKTOP_CREATEWINDOW |
		DESKTOP_ENUMERATE |
		DESKTOP_HOOKCONTROL |
		DESKTOP_WRITEOBJECTS |
		DESKTOP_READOBJECTS |
		DESKTOP_SWITCHDESKTOP |
		GENERIC_WRITE);
	char m_deskName[260];
	memset(m_deskName, 0, sizeof(m_deskName));

	if (GetUserObjectInformation(inputDesktop, UOI_NAME, m_deskName, sizeof(m_deskName), 0)) {
		if (strcmp(m_deskName, "Winlogon") == 0) {

			CloseDesktop(inputDesktop);
			inputDesktop = NULL;
			return true;
		}
	}
	CloseDesktop(inputDesktop);
	inputDesktop = NULL;
	return false;
   
}



void YangScreen::toLowerCase(char* str)
{
	size_t length = strlen(str);
	for (size_t i = 0; i < length; i++) {
		if (isalpha(str[i]))
			str[i] = tolower(str[i]);

	}
}
bool YangScreen::isRdpSession(FILE* file, DWORD* ssid)
{
	PWTS_SESSION_INFO sessionInfo = 0;
	DWORD count = 0;
	DWORD sessionId = 0;
	yangbool err = yangfalse;

	if (WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &sessionInfo, &count)) {
		for (DWORD i = 0; i < count; i++) {
			if (sessionInfo[i].State == WTSActive) {
				std::string cs(sessionInfo[i].pWinStationName);

				char str[256] = { 0 };
				sprintf(str, "%s", sessionInfo[i].pWinStationName);
				toLowerCase(str);
				if (strstr(str, "rdp")) {
					err = yangtrue;
					if (ssid) *ssid = sessionInfo[i].SessionId;
				}
				if (file) {
					fwrite("\n", 1, 1, file);
					fwrite(str, strlen(str), 1, file);
					memset(str, 0, sizeof(str));
					sprintf(str, ",err==%s", err ? "true" : "false");
					fwrite(str, strlen(str), 1, file);
					fflush(file);
				}

			}
		}
		WTSFreeMemory(sessionInfo);
	}



	return err;
}


void YangScreen::setStation() {
	HWINSTA winSta = OpenWindowStation("WinSta0", TRUE, GENERIC_ALL);

	if (winSta == 0) {
		yang_error("\nopen desktop WinSta0==NULL");
		return;
	}

	if (SetProcessWindowStation(winSta) == 0) {
		yang_error("\nbinding desktop WinSta0 fail");
		CloseWindowStation(winSta);
		return;
	}

	CloseWindowStation(winSta);
}


int32_t YangScreen::toCurrent(bool isWinlogon) {
	if (!isWinlogon&&isRdpSession(NULL,NULL)) setStation();
	HDESK inputDesk = OpenInputDesktop(DF_ALLOWOTHERACCOUNTHOOK, FALSE, GENERIC_ALL);
	if (inputDesk == NULL)
	{
		DWORD tempError = GetLastError();
		return 1;
	}

	if (!SetThreadDesktop(inputDesk))
	{
		DWORD tempError = GetLastError();
		CloseDesktop(inputDesk);
		return 1;
	}

	CloseDesktop(inputDesk);
	return Yang_Ok;
}
int32_t YangScreen::toDefault()
{
	HDESK 	desktop = OpenDesktop("Default", 0, TRUE,
		DESKTOP_CREATEMENU |
		DESKTOP_CREATEWINDOW |
		DESKTOP_ENUMERATE |
		DESKTOP_HOOKCONTROL |
		DESKTOP_WRITEOBJECTS |
		DESKTOP_READOBJECTS |
		DESKTOP_SWITCHDESKTOP |
		GENERIC_WRITE);
	bool result = SetThreadDesktop(desktop) != 0;
	CloseDesktop(desktop);
	return int32_t(0);
}

int32_t YangScreen::toWinlogon()
{
	HDESK desktop = OpenDesktop("Winlogon", 0, TRUE,
		DESKTOP_CREATEMENU |
		DESKTOP_CREATEWINDOW |
		DESKTOP_ENUMERATE |
		DESKTOP_HOOKCONTROL |
		DESKTOP_WRITEOBJECTS |
		DESKTOP_READOBJECTS |
		DESKTOP_SWITCHDESKTOP |
		GENERIC_WRITE);

	bool result = SetThreadDesktop(desktop) != 0;
	CloseDesktop(desktop);
	return int32_t(0);
}

bool YangScreen::queryProcess(DWORD processId) {
	bool ret = false;
	PWTS_PROCESS_INFO ppi;
	DWORD dwCounts;
	BOOL bRet = WTSEnumerateProcesses(WTS_CURRENT_SERVER_HANDLE, 0,1,& ppi,& dwCounts);
	if (!bRet)
		return false;

	PWTS_PROCESS_INFO ppiTemp = ppi;
	char str[1024];
	for (int i = 0; i < dwCounts; i++)
	{
		if (ppi->ProcessId == processId) ret = true;
		ppi++;
	}

	WTSFreeMemory(ppiTemp);
	return ret;
}
#endif
