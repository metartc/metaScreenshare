//
// Copyright (c) 2019-2023 yanggaofeng
//

#pragma region Includes
#include <stdio.h>
#include <windows.h>
#include <time.h>
#include "ServiceBase.h"
#include "YangScreenService.h"
#include "yangscreen.h"
#pragma endregion
#include <yangutil/sys/YangLog.h>

#include <yangipc/YangIpcScreenHandle.h>

#include "YangScreenServer.h"
#include "YangScreenTcpServer.h"
#include "YangScreenCaptureWin.h"

#define SERVICE_NAME             "YangScreenService"

YangScreenServer* g_yang_server = NULL;
YangIpcScreenHandle* g_yang_ipcHandle = NULL;
YangScreenTcpServer* g_yang_tcpserver = NULL;
YangScreenCaptureWin* g_yang_capture = NULL;
bool g_fStopping = false;
yangbool yang_ipc_encoder_enable() {
    if (g_yang_ipcHandle && g_yang_ipcHandle->isAlive()) return yangtrue;

    return yangfalse;
}
void g_yang_startScreenListen(char* szPath) {
   
    if (g_yang_server == NULL)  
        g_yang_server = new YangScreenServer(szPath);

    g_yang_ipcHandle = YangIpcScreenHandle::createIpcHandle(&g_yang_server->m_context->avinfo);
 
    g_yang_ipcHandle->init(&g_yang_server->m_msg);
  
    g_yang_ipcHandle->start();

    if (g_yang_tcpserver == NULL) 
        g_yang_tcpserver = new YangScreenTcpServer();

    g_yang_tcpserver->start();
    g_yang_server->m_tcpServer = g_yang_tcpserver;
   
}

void g_yang_startCaptureListen(char* szPath) {
    if (g_yang_capture == NULL) g_yang_capture = new YangScreenCaptureWin(szPath);
    g_yang_capture->start();
}

#define Yang_Screen_Test 0

int main(int argc, char* argv[])
{
     if (argc == 3) {
        char szExePath[260] = { 0 };
        char szPath[260] = { 0 };
        int pargc = argc;
        // int pargc = 2;
      //  YangIpcDataMsg msg;

        if (GetModuleFileName(NULL, szExePath, ARRAYSIZE(szExePath)) != 0)
        {
            std::string s(szExePath);
            int pos = s.rfind('\\');

            memcpy(szPath, s.c_str(), pos + 1);
        }
        if (strcmp(argv[1], "-runas") == 0 && strcmp(argv[2], "screenLoginServer") == 0) {
            printf("\nstart winlogon listen\n");
            g_yang_startCaptureListen(szPath);
        }

        if (strcmp(argv[1], "-runas") == 0 && strcmp(argv[2], "screenServer") == 0) {
            printf("\nstart default listen\n");
            g_yang_startScreenListen(szPath);
        }

        if (strcmp(argv[1], "-test") == 0 && strcmp(argv[2], "testserver") == 0) {
            printf("\nstart test server listen\n");
            YangScreenTcpServer server;
            server.startTestListen();
        }else{

            // Periodically check if the service is stopping.
            while (!g_fStopping)
            {
               Sleep(1000);  // Simulate some lengthy operations.
            }
        }

        yang_delete(g_yang_ipcHandle);

        yang_delete(g_yang_server);
        yang_delete(g_yang_tcpserver);
        yang_delete(g_yang_capture);
        WSACleanup();

    }else
    {
#if  Yang_Screen_Test
        char szExePath[260] = { 0 };
        char szPath[260] = { 0 };
        int pargc = argc;
        // int pargc = 2;
      //  YangIpcDataMsg msg;

        if (GetModuleFileName(NULL, szExePath, ARRAYSIZE(szExePath)) != 0)
        {
            std::string s(szExePath);
            int pos = s.rfind('\\');

            memcpy(szPath, s.c_str(), pos + 1);
        }
      
        g_yang_startScreenListen(szPath);
     
        while (!g_fStopping)
        {
            Sleep(1000);  // Simulate some lengthy operations.
        }
      

    yang_delete(g_yang_ipcHandle);

    yang_delete(g_yang_server);
    yang_delete(g_yang_tcpserver);
    yang_delete(g_yang_capture);
#else
      
        yang_log_info("Parameters:\n");
        yang_log_info(" -install  to install the service.\n");
        yang_log_info(" -remove   to remove the service.\n");

        YangScreenService service(SERVICE_NAME);
        if (!CServiceBase::Run(service))
        {
            yang_log_info("Service failed to run w/err 0x%08lx\n", GetLastError());
        }
      
#endif
    }

    return 0;
}