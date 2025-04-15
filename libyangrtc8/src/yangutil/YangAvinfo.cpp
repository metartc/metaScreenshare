//
// Copyright (c) 2019-2023 yanggaofeng
//
#include <yangutil/yangavinfotype.h>
extern "C"{
	#include <yangutil/sys/YangIni.h>
	#include <yangutil/sys/YangLog.h>
}


class YangInitContext {
public:
	YangInitContext() {
	}

	~YangInitContext() {
	}


};

YangContext::YangContext() {

	memset(&sendRtcPli,0,sizeof(YangSendRequestPli));


	memset(&channeldataRecv,0,sizeof(YangChannelDataRecvI));
	memset(&channeldataSend,0,sizeof(YangChannelDataSendI));


    avinfo.mqtt=(YangMqttInfo*)yang_calloc(sizeof(YangMqttInfo),1);
    avinfo.file=NULL;
	init();
#if Yang_OS_ANDROID
	nativeWindow=NULL;
#endif

}

YangContext::~YangContext() {
	yang_closeLogFile();
	yang_free(avinfo.mqtt);
	yang_free(avinfo.file);
}

void YangContext::init(char *filename) {
#if  !(Yang_OS_ANDROID||Yang_OS_IOS)
	YangIni ini;
	ini.filename=NULL;
	yang_create_ini(&ini,filename);
	ini.initAudio(ini.filename,&avinfo.audio);
	ini.initVideo(ini.filename,&avinfo.video);
	ini.initSys(ini.filename,&avinfo.sys);
	ini.initEnc(ini.filename,&avinfo.enc);
	ini.initRtc(ini.filename,&avinfo.rtc);
	if(avinfo.mqtt)
		ini.initMqtt(ini.filename,avinfo.mqtt);
	if(avinfo.file)
		ini.initFile(ini.filename,avinfo.file);
	initExt(&ini);
	yang_destroy_ini(&ini);
#else
	yang_init_avinfo(&avinfo);
#endif
}

void YangContext::initMqttInfo(char *filename){
#if  !(Yang_OS_ANDROID||Yang_OS_IOS)
	YangIni ini;
	ini.filename=NULL;
	yang_create_ini(&ini,filename);

	if(avinfo.mqtt==NULL)
		avinfo.mqtt=(YangMqttInfo*)yang_calloc(sizeof(YangMqttInfo),1);

	ini.initMqtt(ini.filename,avinfo.mqtt);

	yang_destroy_ini(&ini);
#endif
}
void YangContext::initFileInfo(char *filename){
#if  !(Yang_OS_ANDROID||Yang_OS_IOS)
	YangIni ini;
	ini.filename=NULL;
	yang_create_ini(&ini,filename);
	if(avinfo.file==NULL)
		avinfo.file=(YangFileInfo*)yang_calloc(sizeof(YangFileInfo),1);

	ini.initFile(ini.filename,avinfo.file);
	yang_destroy_ini(&ini);
#endif
}
void YangContext::init() {
    yang_init_avinfo(&avinfo);
    initExt();
}


void YangContext::initExt() {

}
void YangContext::initExt(void *filename) {

}

YangBufferManager::YangBufferManager() {
	m_curindex = 0;
	m_size = 0;
	m_unitsize = 0;
	m_cache = NULL;
}
YangBufferManager::YangBufferManager(int32_t num, int32_t unitsize) {
	m_curindex = 0;
	m_size = 0;
	m_unitsize = 0;
	m_cache = NULL;
	init(num, unitsize);
}
YangBufferManager::~YangBufferManager() {
	yang_deleteA(m_cache);
}

void YangBufferManager::init(int32_t num, int32_t unitsize) {
	m_size = num;
	m_unitsize = unitsize;
	if (m_cache == NULL)
		m_cache = new uint8_t[m_unitsize * m_size];
}
uint8_t* YangBufferManager::getBuffer() {
	if (!m_cache)
		return NULL;
	if (m_curindex >= m_size)
		m_curindex = 0;
	return m_cache + m_unitsize * m_curindex++;
}
