//
// Copyright (c) 2019-2023 yanggaofeng
//



#include "YangKeyboard.h"
#include <yangutil/sys/YangLog.h>
#include <yangscreen/YangScreen.h>
#include <yangutil/sys/YangLoadLib.h>
#include "atlsecurity.h"

typedef VOID(WINAPI* SendSas)(BOOL asUser);
unsigned __stdcall SimulateCtrlAltDelThreadFn( void* pArguments );


YangKeyboard::YangKeyboard(int vkey,STATE state)
:m_nVKey(vkey)
,m_eState(state)
{
}
YangKeyboard::YangKeyboard()	
{
	m_isStart = yangtrue;
	m_isWin7 = yangfalse;
}

YangKeyboard::~YangKeyboard(void)
{
}
bool YangKeyboard::Input(int vk,STATE state)
{
	INPUT   input;
	ZeroMemory(&input,sizeof(INPUT));
	input.type		 = INPUT_KEYBOARD;
	input.ki.wVk	 = vk;  
	input.ki.wScan	 = MapVirtualKey(vk, 0);
	input.ki.dwFlags = (state ==down) ? 0:KEYEVENTF_KEYUP;  
	if(!SendInput(1,   &input,   sizeof(INPUT)))
	{
		yang_error("SendInput %d failed with %d",vk,GetLastError());
		return false;
	}
	
	return true;
}


void YangKeyboard::stop() {

}

void YangKeyboard::run() {
	if (m_isWin7) {
		HWND hwndCtrlAltDel = FindWindow(("SAS window class"), ("SAS window"));
		if (hwndCtrlAltDel == NULL) {
			hwndCtrlAltDel = HWND_BROADCAST;
		}
		PostMessage(hwndCtrlAltDel, WM_HOTKEY, 0, MAKELONG(MOD_ALT | MOD_CONTROL, VK_DELETE));
		return;
	}
	YangLoadLib sasLib;
	sasLib.loadSysObject("sas.dll");

	SendSas sendSas = (SendSas)sasLib.loadFunction("SendSAS");
	if (sendSas == NULL) {
		yang_error("The SendSAS function has not been found");
		m_isStart = yangfalse;
		return;
	}
	sendSas(FALSE); // Try only under service
	m_isStart = yangfalse;
}

void YangKeyboard::Input()
{

	
	// Simulate Ctrl + Alt + Del
	if (IsCtrlAltDel())
	{
		//release pressed key,because is SimulateCtrlAltDel switch desktop,
		//then the "key up event" will send to "winlogon" desktop

		if ((GetKeyState (VK_DELETE) & 0x8000) )	Input(VK_DELETE,up);
		if ((GetKeyState (VK_LMENU) & 0x8000) )		Input(VK_LMENU,up);
		if ((GetKeyState (VK_RMENU) & 0x8000) )		Input(VK_RMENU,up);
		if ((GetKeyState (VK_LCONTROL) & 0x8000) )	Input(VK_LCONTROL,up);
		if ((GetKeyState (VK_RCONTROL) & 0x8000) )	Input(VK_RCONTROL,up);

	
		SimulateCtrlAltDel();
	}
	return;
}
bool YangKeyboard::IsCtrlAltDel()
{
	if(m_eState != down) return false;

	if( m_nVKey == VK_DELETE &&
		((GetKeyState (VK_LMENU) & 0x8000)  || (GetKeyState (VK_RMENU) & 0x8000) )&&
		((GetKeyState (VK_LCONTROL) & 0x8000)  || (GetKeyState (VK_RCONTROL) & 0x8000) )
	)
	{
		return true;
	}
	return false;
}

// if os <= vista 
//		simulate CtrAltDel by posting a WM_HOTKEY message to all the windows on the Winlogon desktop.
// else if create a cad.exe process succeed
//		done
// else 
//		start Windows Task Manager

bool YangKeyboard::SimulateCtrlAltDel()
{
	yang_info("\nSimulateCtrlAltDel");
	OSVERSIONINFO osversioninfo;

	osversioninfo.dwOSVersionInfoSize = sizeof(osversioninfo);
	GetVersionEx(&osversioninfo);

	if (osversioninfo.dwMajorVersion >=6) // >vista
	{
		// if OS >= vista ,we need a cad.exe to simulate CtrlAtlDel
		CAccessToken token;
		if(!token.GetProcessToken(TOKEN_ALL_ACCESS))
		{
			yang_error("GetProcessToken with %d",GetLastError());
			return false;
		}

		char szComd[MAX_PATH];
		char curpath[MAX_PATH];
		GetModuleFileName( 0, curpath, MAX_PATH );
		PathRemoveFileSpec(curpath);
		sprintf(szComd,"\"%s\\cad.exe\"",curpath);

		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);
		ZeroMemory( &pi, sizeof(pi) );
		if(!CreateProcess(0,szComd,0,0,0,NORMAL_PRIORITY_CLASS|CREATE_NO_WINDOW ,0,0,&si,&pi )){
			yang_error(("CreateProcess failed, error code = %d"),GetLastError());
			Input(VK_LCONTROL,down);
			Input(VK_LSHIFT,down);
			Input(VK_ESCAPE,down);
			Input(VK_ESCAPE,up);
			Input(VK_LSHIFT,up);
			Input(VK_LCONTROL,up);
			return true;
		}
		else
		{
			if(WaitForSingleObject(pi.hThread,5000) == WAIT_TIMEOUT)
			{
				TerminateProcess(pi.hProcess,0);
			}
			CloseHandle( pi.hProcess );
			CloseHandle( pi.hThread );
			return true;
		}
		
	}
	
	unsigned threadID;
	HANDLE hThread = (HANDLE)_beginthreadex( NULL, 0, &SimulateCtrlAltDelThreadFn, 0, 0, &threadID );
	CloseHandle(hThread);
	return TRUE;
}

unsigned __stdcall SimulateCtrlAltDelThreadFn( void* pArguments )
{
	HDESK old_desktop = GetThreadDesktop(GetCurrentThreadId());

	if(old_desktop == NULL ){
		yang_error("GetThreadDesktop with %d",GetLastError());
		return FALSE;
	}
	if (!YangScreen::selectDesktop("Winlogon"))
	{
		yang_error(("SelectDesktop(\"Winlogon\") with %d"),GetLastError());
		return FALSE;
	}

	// Winlogon uses hotkeys to trap Ctrl-Alt-Del...
	if(!PostMessage(HWND_BROADCAST, WM_HOTKEY, 0, MAKELONG(MOD_ALT | MOD_CONTROL, VK_DELETE)))
		yang_error("PostMessage(HWND_BROADCAST) with %d",GetLastError());

	// Switch back to our original desktop
	//if (old_desktop != NULL)	SelectHDESK(old_desktop);
	
	return 0;

}
