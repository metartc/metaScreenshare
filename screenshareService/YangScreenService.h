//
// Copyright (c) 2019-2023 yanggaofeng
//

#include "ServiceBase.h"
#include <stdio.h>


class YangScreenService : public CServiceBase
{
public:

    YangScreenService(PSTR pszServiceName, 
        BOOL fCanStop = TRUE, 
        BOOL fCanShutdown = TRUE, 
        BOOL fCanPauseContinue = FALSE);
    virtual ~YangScreenService(void);

    void createProcess(bool loginState,char* cmd,char* curPath);

    HANDLE m_proc1;
    HANDLE m_proc2;
    DWORD m_procId1;

protected:

    virtual void OnStart(DWORD dwArgc, PSTR *pszArgv);
    virtual void OnStop();

    void ServiceWorkerThread(void);

private:
    
    
    BOOL m_fStopping;
    HANDLE m_hStoppedEvent;
  //  FILE* m_file = NULL;
};