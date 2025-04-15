//
// Copyright (c) 2019-2023 yanggaofeng
//
#include "YangScreenServer.h"
#include <yangcapture/YangMouse.h>
#include <yangutil/yangavinfo.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangIni.h>
#include <yangutil/sys/YangFile.h>
#include <yangutil/sys/YangSocket.h>
#include <yangutil/sys/YangString.h>

#include <string>

void g_yang_p2p_receiveData(void* context, int32_t uid, uint8_t* data, int32_t len) {
    YangScreenServer* win = (YangScreenServer*)context;
    if (win == NULL || win->m_tcpServer == NULL) return;
    
    if (win->m_tcpServer->loginState()) 
        win->m_tcpServer->sendKeyMouseMessage((char*)data, len);
    else
        win->m_screenEvent->screenEvent((char*)data, len);

}

YangScreenServer::YangScreenServer(char* filepath)
{
    initIni(filepath);

    m_context->avinfo.sys.httpPort = 1988;
    m_context->avinfo.sys.mediaServer = Yang_Server_P2p;
    m_context->avinfo.rtc.rtcLocalPort = 10000 + yang_random() % 15000;

    m_isDrawmouse = true; //screen draw mouse
    m_screenInternal = 33;

    m_isStartpush = 0;
    m_context->avinfo.video.videoEncoderType = Yang_VED_H264;



    m_isStartpush = false;
    m_hasAudio = false;

    m_msg.user = this;
    m_msg.receiveMsg = g_yang_p2p_receiveData;

    m_screenEvent = new YangScreenEvent(m_context);
    m_tcpServer = NULL;
}

YangScreenServer::~YangScreenServer()
{
    closeAll();
}
void YangScreenServer::initIni(char* szPath) {
    char inifile[260] = { 0 };
    char logfile[260] = { 0 };
    sprintf(inifile, "%sdesktop_config.ini", szPath);
    sprintf(logfile, "%syang_log.log", szPath);
        
    m_context = new YangContext();
    YangIni ini;
    ini.filename = NULL;
    yang_create_ini2(&ini, inifile);
    ini.initAvinfo(ini.filename, &m_context->avinfo);
    yang_destroy_ini(&ini);
  
   // m_ini->avinfo.video.videoEncoderFormat = YangI420;


    m_context->avinfo.sys.mediaServer = Yang_Server_P2p;
    m_context->avinfo.rtc.rtcLocalPort = 10000 + yang_random() % 15000;
 
    //using gpu encode
    if (m_context->avinfo.video.videoEncHwType > 0) {
        m_context->avinfo.video.videoEncHwType = YangV_Hw_Nvdia;//YangV_Hw_Intel,  YangV_Hw_Nvdia,
        //m_ini->avinfo.video.videoEncoderFormat=YangI420;
        m_context->avinfo.video.videoCaptureFormat = YangArgb;//YangI420;//YangArgb;
       
    }
    else {
        m_context->avinfo.video.videoEncHwType = Yang_Hw_Soft;
        // m_ini->avinfo.video.videoEncoderFormat=YangI420;
        m_context->avinfo.video.videoCaptureFormat = YangI420;
      
    }
    initPara();
    yang_setLogLevel(m_context->avinfo.sys.logLevel);
    
    yang_setLogFile2(m_context->avinfo.sys.enableLogFile, logfile);
    yang_setLogFileId(1988);
    yang_trace("metaScreenshare demo is starting!\n");
}
void YangScreenServer::init()
{


}


void YangScreenServer::initPara() {
  
    m_context->avinfo.video.videoCacheNum = 10;
    m_context->avinfo.video.evideoCacheNum = 10;
    m_context->avinfo.video.videoPlayCacheNum = 10;

    m_context->avinfo.rtc.rtcLocalPort = 17000;
    m_context->avinfo.enc.enc_threads = 4;
}



void YangScreenServer::closeAll() {

    if (m_context) {
      
        yang_delete(m_context);
        yang_delete(m_screenEvent);

    }
}






