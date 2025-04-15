
//
// Copyright (c) 2019-2023 yanggaofeng
//
#pragma region Includes
#include "YangScreenService.h"
#include "ThreadPool.h"
#include <stdio.h>
#pragma endregion

#include "yangscreen.h"
#include <string>
#include "YangProcess.h"

#include "userenv.h"

#include "wtsapi32.h"

#include  <yangscreen/YangScreen.h>
#include <yangutil/sys/YangThread.h>
YangScreenService::YangScreenService(PSTR pszServiceName, 
                               BOOL fCanStop, 
                               BOOL fCanShutdown, 
                               BOOL fCanPauseContinue)
: CServiceBase(pszServiceName, fCanStop, fCanShutdown, fCanPauseContinue)
{
    m_fStopping = FALSE;
  //  m_file = NULL;
    m_proc1 = NULL;
    m_proc2 = NULL;
    m_procId1 = -1;
    // Create a manual-reset event that is not signaled at first to indicate 
    // the stopped signal of the service.
    m_hStoppedEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (m_hStoppedEvent == NULL)
    {
        throw GetLastError();
    }

}


YangScreenService::~YangScreenService(void)
{
    if (m_hStoppedEvent)
    {
        CloseHandle(m_hStoppedEvent);
        m_hStoppedEvent = NULL;
    }
}


void YangScreenService::createProcess(bool loginState, char* cmd,char* curPath) {

    HANDLE hToken = NULL;
    HANDLE hTokenDup = NULL;
    LPVOID  pEnv = NULL;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    DWORD ssid = -1;
    bool isRdp = YangScreen::isRdpSession(NULL, &ssid);

    HANDLE hThisProcess = GetCurrentProcess();
    BOOL bResult = FALSE;
    bResult = OpenProcessToken(hThisProcess, TOKEN_ALL_ACCESS, &hToken);

  

    //复制Token
    if (!DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary, &hTokenDup))
    {
        CloseHandle(hToken);
        return;
    }
    DWORD dwSessionId = WTSGetActiveConsoleSessionId();
    if (isRdp && !loginState) {
        dwSessionId = ssid;
    }

    bResult = SetTokenInformation(hTokenDup, TokenSessionId, &dwSessionId, sizeof(DWORD));
   
    DWORD UIAccess = 1;
    SetTokenInformation(hTokenDup, TokenUIAccess, &UIAccess, sizeof(DWORD));
    //获取环境信息
    if (!CreateEnvironmentBlock(&pEnv, hTokenDup, FALSE))
    {
        CloseHandle(hToken);
        CloseHandle(hTokenDup);
        return;
    }
  

    //设置启动参数信息
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    if (loginState)
        si.lpDesktop = "winsta0\\winlogon";
    else
        si.lpDesktop = "winsta0\\default";
    ZeroMemory(&pi, sizeof(pi));
    DWORD dwCreationFlag = HIGH_PRIORITY_CLASS | CREATE_UNICODE_ENVIRONMENT | CREATE_NO_WINDOW;// CREATE_NEW_CONSOLE;//HIGH_PRIORITY_CLASS NORMAL_PRIORITY_CLASS | CREATE_UNICODE_ENVIRONMENT
  

    if (!CreateProcessAsUser(hTokenDup, NULL, cmd, NULL, NULL, FALSE, dwCreationFlag, pEnv, curPath, &si, &pi))
    {
        char err[512] = { 0 };
        sprintf(err, "\ncreateProcess error(%lu)>>>>>>%s\n", GetLastError(), cmd);
       // yang_printf(m_file, err);
        DestroyEnvironmentBlock(pEnv);
        CloseHandle(hTokenDup);
        CloseHandle(hToken);
      
    }
    if (loginState) {
        m_proc1 = pi.hProcess;
        m_procId1 = pi.dwProcessId;
    }
    else
        m_proc2 = pi.hProcess;

    WaitForSingleObject(pi.hProcess, INFINITE);
 
    //清理工作
    DestroyEnvironmentBlock(pEnv);
    CloseHandle(hTokenDup);
    CloseHandle(hToken);


}


void YangScreenService::OnStart(DWORD dwArgc, LPSTR *lpszArgv)
{
    // Log a service start message to the Application log.
    WriteEventLogEntry("YangScreenService in OnStart", 
        EVENTLOG_INFORMATION_TYPE);
 
    
    CThreadPool::QueueUserWorkItem(&YangScreenService::ServiceWorkerThread, this);
}

DWORD WINAPI yang_run_service_thread(LPVOID lpParam) {

    YangScreenService* server = (YangScreenService*)lpParam;
 

    char szExePath[260] = { 0 };
    char szPath[260] = { 0 };
    char cmd[260] = { 0 };
    if (GetModuleFileName(NULL, szExePath, ARRAYSIZE(szExePath)) != 0)
    {
        std::string s(szExePath);
        int pos = s.rfind('\\');

        memcpy(szPath, s.c_str(), pos + 1);
    }
    //Sleep(2000);
    sprintf(cmd, "\"%sYangScreenService.exe\"  -runas screenLoginServer", szPath);
    server->createProcess(true,cmd, szPath);
    return NULL;
}
DWORD WINAPI yang_run_service_thread2(LPVOID lpParam) {
    YangScreenService* server = (YangScreenService*)lpParam;

    char szExePath[260] = { 0 };
    char szPath[260] = { 0 };
    char cmd[260] = { 0 };
    if (GetModuleFileName(NULL, szExePath, ARRAYSIZE(szExePath)) != 0)
    {
        std::string s(szExePath);
        int pos = s.rfind('\\');

        memcpy(szPath, s.c_str(), pos + 1);
    }
    sprintf(cmd, "\"%sYangScreenService.exe\"  -runas screenServer", szPath);
    server->createProcess(false, cmd, szPath);
    return NULL;
}

void YangScreenService::ServiceWorkerThread(void)
{
   CreateThread( NULL,0,yang_run_service_thread,this,  0,  NULL);
   CreateThread(NULL, 0, yang_run_service_thread2, this, 0, NULL);

  boolean isRdp = YangScreen::isRdpSession(NULL, NULL);
  boolean winlogonRunning = true;
  while (!m_fStopping)
  {         
        if (isRdp) {
            if (!YangScreen::isRdpSession(NULL, NULL)) {
                winlogonRunning= YangScreen::queryProcess(m_procId1);
                if (!winlogonRunning) {
                    Sleep(1000 * 2);
                    CreateThread(NULL, 0, yang_run_service_thread, this, 0, NULL);
                    isRdp = false;
                }             
            }
        }
        Sleep(1000);  // Simulate some lengthy operations.
   }

    // Signal the stopped event.
    SetEvent(m_hStoppedEvent);
}


void YangScreenService::OnStop()
{
    // Log a service stop message to the Application log.
    WriteEventLogEntry("YangScreenService in OnStop", 
        EVENTLOG_INFORMATION_TYPE);
 
    // Indicate that the service is stopping and wait for the finish of the 
    // main service function (ServiceWorkerThread).
    m_fStopping = TRUE;
    if(m_proc1) TerminateProcess(m_proc1, 0);
    if(m_proc2) TerminateProcess(m_proc2, 0);
    if (WaitForSingleObject(m_hStoppedEvent, INFINITE) != WAIT_OBJECT_0)
    {
        throw GetLastError();
    }
}