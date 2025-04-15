//
// Copyright (c) 2019-2023 yanggaofeng
//
#ifndef SRC_YangScreenServer_H_
#define SRC_YangScreenServer_H_

#include <yangcapture/YangMouse.h>
#include <yangipc/YangIpcConfig.h>
#include "yangutil/yangavinfotype.h"


#include "YangScreenEvent.h"
#include "YangScreenTcpServer.h"
class YangScreenServer
{
public:
	YangScreenServer(char* filepath);
	~YangScreenServer();
	void init();

public:
 
    YangScreenEvent* m_screenEvent;
    YangScreenTcpServer* m_tcpServer;
    void closeAll();
    YangContext* m_context;
     bool m_hasAudio;

    YangIpcDataMsg m_msg;
  
private:
    bool m_isDrawmouse;

    int32_t m_screenInternal;
    
  

    int m_screenWidth;
    int m_screenHeight;
    bool m_isStartpush;
    

    
    void initPara();
    void initIni(char* filepath);
};

#endif