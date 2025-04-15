#pragma once
#include <windows.h>
BOOL StartInteractiveClientProcess(
    LPSTR lpszUsername,    // client to log on
    LPSTR lpszDomain,      // domain of client's account
    LPSTR lpszPassword,    // client's password
    LPSTR lpCommandLine    // command line to execute
);
