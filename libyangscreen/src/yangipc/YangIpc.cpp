//
// Copyright (c) 2019-2024 yanggaofeng
//


#include <yangipc/YangIpc.h>

#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangFile.h>
#include <yangutil/sys/YangSocket.h>
#include <yangutil/sys/YangMath.h>


int32_t yang_ipc_receive_setLocalDescription(char *sdp,int32_t uid ,void* user){

	YangIpc *session = (YangIpc*) user;
	
	if (session == NULL || session->m_rtc==NULL)
		return 1;

	return session->m_rtc->setLocalDescription(uid,sdp);
}

int32_t g_ipc_receive_addCandidate(int32_t uid,char* candidate,void* user){
	YangIpc* session = (YangIpc*)user;
	if (session == NULL || session->m_rtc==NULL)
		return 1;
	
	return session->m_rtc->addCandidate(uid,candidate);
}


void g_ipc_receive_close(int32_t uid, void *user) {
	YangIpc *session = (YangIpc*) user;
	if (session == NULL || session->m_rtc==NULL)
		return;

	session->m_rtc->disconnectPeer();

}

void g_yang_ipc_sendRequest(void* context, int32_t puid, uint32_t ssrc, YangRequestType req) {
	YangIpc* session = (YangIpc*)context;
	if (session == NULL || session->m_rtc==NULL)
		return;

	session->m_rtc->sendRequest(puid, ssrc, req);

}


int32_t yang_ipc_receive_addPeer(char* data, int32_t nb_data, char* response, char* remoteIp, int32_t uid, void* user) {

	int32_t err = Yang_Ok,localPort;
	YangIpc* session = (YangIpc*)user;

	if  (session == NULL || session->m_rtc==NULL)
		return 1;

	if(session->m_rtc->m_peer&&session->m_rtc->m_peer->isConnected())
		return 2;

	localPort = session->m_avinfo->rtc.rtcLocalPort++;
	if (session->m_avinfo->rtc.rtcLocalPort > 65000)
		session->m_avinfo->rtc.rtcLocalPort = 15000;
	if (session->m_videoEncoder && !session->m_isInitRtc) {

		session->m_videoEncoder->init(session->m_videoEncoder->session,
				&session->m_rtc->m_encoderEnable, &session->m_rtc->m_encoderCallback);
		session->m_videoEncoder->start(session->m_videoEncoder->session);

		session->m_isInitRtc = yangtrue;
	}


	session->m_rtc->start();

	err = session->m_rtc->connect(data, response, remoteIp, localPort, uid);

	if (err != Yang_Ok) {
		return yang_error_wrap(err, " add peer failure!");
	}

	return err;
}

int32_t  YangIpc::getIpcHandle(YangIpcHandle *handle){

	handle->user=this;
	handle->connect=yang_ipc_receive_addPeer;
	handle->setLocalDescription=yang_ipc_receive_setLocalDescription;
	handle->addCandidate=g_ipc_receive_addCandidate;
	handle->close=g_ipc_receive_close;
	return Yang_Ok;
}

int32_t YangIpc::start(){

	return m_mqttServer->start(m_mqttServer->session,(char*)Yang_Topic_Server);

}

int32_t YangIpc::init(){

	return Yang_Ok;
}

void YangIpc::checkAlive(){
	
	if(m_rtc)
		m_rtc->checkAlive();

}

yangbool YangIpc::isAlive(){

	if(m_rtc){
		return m_rtc->isAlive();
	}
	return yangfalse;
}


int32_t YangIpc::stop(){
	m_rtc->stop();
	return Yang_Ok;
}

int32_t yang_ipc_addIceCandidate(void* user, int32_t uid, char* candidateStr) {
	YangIpc* ipc = (YangIpc*)user;
	if(ipc == NULL || candidateStr == NULL)
		return 1;

	return ipc->m_mqttServer->addCandidate(ipc->m_mqttServer->session, uid, candidateStr);
}

YangIpc::YangIpc(YangIpcConfig* config){

	m_enableP2p=yangfalse;
	m_isInitRtc=yangfalse;

	m_videoEncoder=config->videoEncoder;
	m_avinfo = config->avinfo;
	m_avinfo->rtc.enableHttpServerSdp = yangfalse;


	if(config->dataCallback){
		m_dataCallback.user=config->dataCallback->user;
		m_dataCallback.addIceCandidate=config->dataCallback->addIceCandidate;
	}
	else {
		m_dataCallback.user = this;
		m_dataCallback.addIceCandidate = yang_ipc_addIceCandidate;
	}
	
	m_dataMsg=config->dataMsg;
	
	m_rtc=new YangIpcRtc(config->avinfo,&m_dataCallback, m_dataMsg);
	m_rtc->m_rtcCallback.context=this;
	m_rtc->m_rtcCallback.sendRequest=g_yang_ipc_sendRequest;
	m_rtc->m_videoEncoder = m_videoEncoder;

	getIpcHandle(&m_ipcHandle);
	m_mqttServer = (YangIpcMessageServer*)yang_calloc(sizeof(YangIpcMessageServer), 1);
	yang_create_ipcmessageServer(m_mqttServer, &m_ipcHandle, m_dataMsg, m_avinfo);
	config->avinfo->rtc.enableHttpServerSdp=yangfalse;
	
}

YangIpc::~YangIpc(){
	yang_delete(m_rtc);

	if(m_mqttServer){
		yang_destroy_ipcmessageServer(m_mqttServer);
		yang_free(m_mqttServer);
	}
	yang_closeLogFile();
}


