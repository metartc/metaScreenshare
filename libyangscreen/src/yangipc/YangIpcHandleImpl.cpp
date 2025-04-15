//
// Copyright (c) 2019-2023 yanggaofeng
//

#include "YangIpcHandleImpl.h"


void g_yang_ipchandle_doTask(int32_t taskId, void *user) {
	if (user == NULL)
		return;

	YangIpcHandleImpl *ipc = (YangIpcHandleImpl*) user;
	if (taskId == 5) {
        ipc->m_ipc->m_rtc->checkAlive();
	}
}


YangIpcHandleImpl::YangIpcHandleImpl(YangAVInfo* avinfo) {
	m_timer_5s = NULL;
	m_ipc = NULL;

	m_videoEncoder = NULL;
	m_isInit = yangfalse;
	m_avinfo = avinfo;
}

YangIpcScreenHandle* YangIpcScreenHandle::createIpcHandle(YangAVInfo* avinfo){
	return new YangIpcHandleImpl(avinfo);
}

YangIpcHandleImpl::~YangIpcHandleImpl() {
	stopCtimer();
	yang_delete(m_ipc);
	if (m_videoEncoder) {
		yang_destroy_videoEncoder(m_videoEncoder);
		yang_free(m_videoEncoder);

	}
}

yangbool YangIpcHandleImpl::isAlive(){
	if(m_ipc)
		return m_ipc->isAlive();
	return yangfalse;
}

void YangIpcHandleImpl::init(YangIpcDataMsg* channelmsg) {
	
	m_videoEncoder = (YangEncoderVideo*) yang_calloc(sizeof(YangEncoderVideo), 1);
	yang_create_videoEncoder(m_videoEncoder, m_avinfo);
	
	YangIpcConfig config;
	memset(&config,0,sizeof(YangIpcConfig));
	config.ipcServerType=m_avinfo->sys.ipcServerType;
	config.avinfo=m_avinfo;
	
	config.videoEncoder=m_videoEncoder;
	config.dataMsg=channelmsg;
	
	//config.dataCallback = &m_dataCallback;
	m_ipc=new YangIpc(&config);
	
}
void YangIpcHandleImpl::initCtimer() {
	m_timer_5s = (YangCTimer*) calloc(1, sizeof(YangCTimer));
	yang_create_timer(m_timer_5s, this, 5, 5000);
	m_timer_5s->doTask = g_yang_ipchandle_doTask;

	yang_timer_start(m_timer_5s);
}
void YangIpcHandleImpl::stopCtimer() {
	yang_timer_stop(m_timer_5s);
	yang_destroy_timer(m_timer_5s);
	yang_free(m_timer_5s);
}


yangbool YangIpcHandleImpl::encoder_enable() {
	if ( m_ipc->isAlive())
		return yangtrue;

	return yangfalse;
}

void YangIpcHandleImpl::start() {
	
	m_ipc->init();

	m_ipc->start();
	
	initCtimer();
	
}

