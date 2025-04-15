#include "YangRegReader.h"
#include <yangutil/sys/YangLog.h>

YangRegReader::YangRegReader()
{
}

YangRegReader::~YangRegReader()
{
}

yangbool YangRegReader::isDisableVad()
{
	HKEY hKey;
	int ret = RegOpenKeyExA(HKEY_LOCAL_MACHINE, (char*)"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon", 0, KEY_EXECUTE, &hKey);
	if (ret != ERROR_SUCCESS)
	{
		yang_error("open regedit fail");
		return yangfalse;
	}

	DWORD dwType = REG_DWORD; 
	DWORD cbData = 256;
	char value[256] = { 0 };
	ret = RegQueryValueEx(hKey, (char*)"DisableCad", NULL, &dwType, (LPBYTE)value, &cbData);
	if (ret == ERROR_SUCCESS)
	{
	
		if (*(DWORD*)value == 1) return yangtrue;
	}
	else
	{
		yang_error("read regedit key fail");
		RegCloseKey(hKey);
		return yangfalse;
	}
	RegCloseKey(hKey);

	return yangfalse;
}
