//
// Copyright (c) 2019-2023 yanggaofeng
//
#include "YangUsertoken.h"
#ifdef _MSC_VER
#include <wtsapi32.h>
#include <yangutil/sys/YangLog.h>
YangUsertoken::YangUsertoken(bool isServiceProcess)
{
    if (isServiceProcess) {
      WTSQueryUserToken(-1, &h);
    } else {
      if (!OpenProcessToken(GetCurrentProcess(), GENERIC_ALL, &h)) {
        DWORD err = GetLastError();
        if (err != ERROR_CALL_NOT_IMPLEMENTED)  {
            yang_error("OpenProcessToken failed(%lu)", err);
        }
        h = INVALID_HANDLE_VALUE;
      }
    }
}
YangUsertoken::~YangUsertoken()
{
    if (h) CloseHandle(h);
}

void YangUsertoken::init(){

}



#endif
