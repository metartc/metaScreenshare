//
// Copyright (c) 2019-2023 yanggaofeng
//

#pragma region "Includes"
#include <stdio.h>
#include <windows.h>
#include "ServiceInstaller.h"
#pragma endregion
#include "yangscreen.h"
#include <string>

void InstallService(PSTR pszServiceName, 
                    PSTR pszDisplayName, 
                    DWORD dwStartType,
                    PSTR pszDependencies, 
                    PSTR pszAccount, 
                    PSTR pszPassword)
{
    char szPath[MAX_PATH] = { 0 };
    char szExePath[260] = { 0 };
    char cmd[260] = { 0 };
    SC_HANDLE schSCManager = NULL;
    SC_HANDLE schService = NULL;

    if (GetModuleFileName(NULL, szExePath, ARRAYSIZE(szExePath)) == 0)
    {
        yang_log_error("GetModuleFileName failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }
    else {
        std::string s(szExePath);
        int pos = s.rfind('\\');

        memcpy(szPath, s.c_str(), pos + 1);
    }
    
    sprintf(cmd, "\"%sYangScreenService.exe\"", szPath);
    // Open the local default service control manager database
    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);// SC_MANAGER_CONNECT |  SC_MANAGER_CREATE_SERVICE);
    if (schSCManager == NULL)
    {
        //ERROR_ACCESS_DENIED
        yang_log_error("OpenSCManager failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    // Install the service into SCM by calling CreateService
    schService = CreateService(
        schSCManager,                   // SCManager database
        pszServiceName,                 // Name of service
        pszDisplayName,                 // Name to display
        SERVICE_ALL_ACCESS,           // Desired access
        SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,      // Service type
        dwStartType,                    // Service start type
        SERVICE_ERROR_NORMAL,           // Error control type
        cmd,                         // Service's binary
        NULL,                           // No load ordering group
        NULL,                           // No tag identifier
        pszDependencies,                // Dependencies
        NULL, NULL);
    //    pszAccount,                     // Service running account
   //     pszPassword                     // Password of the account
     //   );
    if (schService == NULL)
    {
        yang_log_error("CreateService failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    yang_log_info("%s is installed\n", pszServiceName);

Cleanup:
    // Centralized cleanup for all allocated resources.
    if (schSCManager)
    {
        CloseServiceHandle(schSCManager);
        schSCManager = NULL;
    }
    if (schService)
    {
        CloseServiceHandle(schService);
        schService = NULL;
    }
}



void UninstallService(PSTR pszServiceName)
{
    SC_HANDLE schSCManager = NULL;
    SC_HANDLE schService = NULL;
    SERVICE_STATUS ssSvcStatus = {};

    // Open the local default service control manager database
    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (schSCManager == NULL)
    {
        yang_log_error("OpenSCManager failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    // Open the service with delete, stop, and query status permissions
    schService = OpenService(schSCManager, pszServiceName, SERVICE_STOP | 
        SERVICE_QUERY_STATUS | DELETE);
    if (schService == NULL)
    {
        yang_log_error("OpenService failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    // Try to stop the service
    if (ControlService(schService, SERVICE_CONTROL_STOP, &ssSvcStatus))
    {
        yang_log_error("Stopping %s.", pszServiceName);
        Sleep(1000);

        while (QueryServiceStatus(schService, &ssSvcStatus))
        {
            if (ssSvcStatus.dwCurrentState == SERVICE_STOP_PENDING)
            {
                yang_log_error(".");
                Sleep(1000);
            }
            else break;
        }

        if (ssSvcStatus.dwCurrentState == SERVICE_STOPPED)
        {
            yang_log_error("\n%s is stopped.\n", pszServiceName);
        }
        else
        {
            yang_log_error("\n%s failed to stop.\n", pszServiceName);
        }
    }

    // Now remove the service by calling DeleteService.
    if (!DeleteService(schService))
    {
        yang_log_error("DeleteService failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    yang_log_error("%s is removed.\n", pszServiceName);

Cleanup:
    // Centralized cleanup for all allocated resources.
    if (schSCManager)
    {
        CloseServiceHandle(schSCManager);
        schSCManager = NULL;
    }
    if (schService)
    {
        CloseServiceHandle(schService);
        schService = NULL;
    }
}

void yang_start_service(PSTR pszServiceName) {
    SC_HANDLE schSCManager = NULL;
    SC_HANDLE schService = NULL;
    SERVICE_STATUS ssSvcStatus = {};

    // Open the local default service control manager database
    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (schSCManager == NULL)
    {
        yang_log_error("OpenSCManager failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    // Open the service with delete, stop, and query status permissions
    schService = OpenService(schSCManager, pszServiceName, SERVICE_START | SERVICE_STOP |
        SERVICE_QUERY_STATUS | DELETE);
    if (schService == NULL)
    {
        yang_log_error("OpenService failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    // Try to stop the service
    if (ControlService(schService, SERVICE_CONTROL_STOP, &ssSvcStatus))
    {
        yang_log_error("Stopping %s.", pszServiceName);
        Sleep(1000);

        while (QueryServiceStatus(schService, &ssSvcStatus))
        {
            if (ssSvcStatus.dwCurrentState == SERVICE_STOP_PENDING)
            {
                yang_log_error(".");
                Sleep(1000);
            }
            else break;
        }

        if (ssSvcStatus.dwCurrentState == SERVICE_STOPPED)
        {
            yang_log_error("\n%s is stopped.\n", pszServiceName);
        }
        else
        {
            yang_log_error("\n%s failed to stop.\n", pszServiceName);
        }
    }
    if (StartService(schService, NULL, NULL) == FALSE)
    {
        yang_log_error("start service error。");
        ::CloseServiceHandle(schService);
        ::CloseServiceHandle(schSCManager);
        return;
    }
    // 等待服务启动
    while (::QueryServiceStatus(schService, &ssSvcStatus) == TRUE)
    {
        ::Sleep(ssSvcStatus.dwWaitHint);
        if (ssSvcStatus.dwCurrentState == SERVICE_RUNNING)
        {
           // AfxMessageBox("start success。");
            ::CloseServiceHandle(schService);
            ::CloseServiceHandle(schSCManager);
            return;
        }
    }
  
Cleanup:
    // Centralized cleanup for all allocated resources.
    if (schSCManager)
    {
        CloseServiceHandle(schSCManager);
        schSCManager = NULL;
    }
    if (schService)
    {
        CloseServiceHandle(schService);
        schService = NULL;
    }
}