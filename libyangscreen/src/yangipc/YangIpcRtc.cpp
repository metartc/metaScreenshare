//
// Copyright (c) 2019-2024 yanggaofeng
//
#include <yangipc/YangIpcRtc.h>
#include <yangstream/YangStreamType.h>
#include <yangvideo/YangNalu.h>
#include <yangvideo/YangMeta.h>


#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangCUrl.h>
#include <yangutil/sys/YangFile.h>
#include <yangutil/sys/YangTimestamp.h>

int32_t YangIpcRtc::disconnectPeer(){
	m_isConnected=yangfalse;
	m_connState = Yang_Conn_State_New;
	return Yang_Ok;
}

void YangIpcRtc::checkAlive() {
	
	if (!m_isConnected)
		return;

	if (m_connState == Yang_Conn_State_Connected&&m_peer&&m_peer->isConnected() && !m_peer->isAlive()) {

		yang_trace("\nremove timeout session");
		disconnectPeer();
	}

}

void* yang_ipc_rtcrecv_start_thread(void *obj) {

	YangIpcRtc* session= (YangIpcRtc*) obj;

	session->m_isStart = yangtrue;
	session->m_isLoop = yangtrue;

	while (session->m_isLoop) {
		if (session->m_peer == NULL) {
			yang_usleep(5000);
			session->m_inited = yangfalse;
			continue;
		}

		if(!session->m_isConnected){
			if(session->m_peer){
				yang_thread_mutex_lock(&session->m_mutex);
				yang_delete(session->m_peer);
				yang_thread_mutex_unlock(&session->m_mutex);
			}
			yang_usleep(5000);
			session->m_inited = yangfalse;
			continue;
		}

		if (session->m_connState != Yang_Conn_State_Connected) {
			yang_usleep(1000 * 5);
			continue;
		}

		if(!session->m_inited&& session->m_pacer){
			session->m_inited = yangtrue;
			session->m_pacer->reset();
		}

		if (session->m_pacer) {
			session->m_pacer->process();
		}else
			yang_usleep(1000 * 5);
	
	}

	session->m_isStart = yangfalse;
	return NULL;
}
void YangIpcRtc::start() {
	if (m_isStart)
		return;

	if (m_enablePacer&&yang_thread_create(&m_threadId, 0, yang_ipc_rtcrecv_start_thread,this)) {
		yang_error("YangThread::start could not start thread");
	}
}

void YangIpcRtc::stop() {
	m_isLoop = yangfalse;
}


int32_t YangIpcRtc::connect( char *sdp,
		char *answer, char *remoteIp, int32_t localPort,int32_t uid) {
	int32_t ret;
	m_isConnected=yangtrue;
	m_param.pushVideo.width = m_avinfo->video.width;
	m_param.pushVideo.height = m_avinfo->video.height;
	m_param.pushVideo.fps = 30;

	m_param.uid = uid;
	m_param.isControlled = yangtrue;
	m_param.rtc.rtcLocalPort = 17000;
	YangPeerConnection8* peer = yang_create_peerConnection8(&m_param,this,this,this,this);

	peer->addAudioTrack(m_avinfo->audio.audioEncoderType);
	peer->addVideoTrack(Yang_VED_H264);
	peer->addTransceiver(YangMediaAudio,YangSendonly);
	peer->addTransceiver(YangMediaVideo,YangSendonly);
	peer->createDataChannel();

	if((ret = peer->setRemoteDescription( sdp))!=Yang_Ok)
		goto cleanup;

	if((ret = peer->createAnswer( answer))!=Yang_Ok)
		goto cleanup;

	yang_thread_mutex_lock(&m_mutex);
	m_peer = peer;
	yang_delete(m_pacer);
	m_pacer=yang_create_rtcPacer(&m_param);
	m_pacer->attachConnection(peer);
	yang_thread_mutex_unlock(&m_mutex);
	return Yang_Ok;
	cleanup:
	yang_delete(peer);
	yang_error("ipc add rtc fail");
	m_isConnected=yangfalse;
	return ret;
}

int32_t YangIpcRtc::addCandidate(int32_t uid,char* candidate){
	
	if (m_peer) {
		m_peer->addIceCandidate(candidate);
	}

	return Yang_Ok;
}

int32_t YangIpcRtc::setLocalDescription(int32_t uid,char* sdp){
	
	if(m_peer)
		m_peer->setLocalDescription(sdp);

	return Yang_Ok;
}

void YangIpcRtc::init(){

}

yangbool YangIpcRtc::isAlive(){
	
	if (m_peer == NULL || !m_isConnected)
		return yangfalse;
	
	if(m_connState == Yang_Conn_State_Connected)
		return yangtrue;
	
	 return yangfalse;
}

void yang_onAudioData(void* psession,YangFrame* audioFrame){

}

void yang_onVideoData(void* psession,YangFrame* videoFrame){
	YangIpcRtc* session=(YangIpcRtc*)psession;

	if(psession==NULL || videoFrame==NULL) return;

	if (session->m_connState != Yang_Conn_State_Connected) {
		session->m_inited = yangfalse;
		return;
	}

	if (session->m_enablePacer) {
		session->m_pacer->bufferVideoFrame(videoFrame->payload, videoFrame->nb, videoFrame->pts, videoFrame->frametype);
	}else{
		if(!session->m_isConnected)
			yang_delete(session->m_peer);
		if(session->m_peer&&session->m_pacer)
			session->m_pacer->writeVideoFrame(videoFrame->payload,videoFrame->nb,videoFrame->pts,videoFrame->frametype);
	}

}

yangbool yang_ipc_rtc_enable(void* psession){
	YangIpcRtc* session=(YangIpcRtc*)psession;
	
	return session->m_connState == Yang_Conn_State_Connected ? yangtrue : yangfalse;

}


void YangIpcRtc::receiveAudio(uint8_t* data,int32_t nb,uint64_t pts){

}
//frametype:YANG_Frametype_I YANG_Frametype_P
void YangIpcRtc::receiveVideo(uint8_t* data,int32_t nb,uint64_t pts,int32_t frametype){

}
void YangIpcRtc::receiveMsg(uint8_t* data,int32_t nb,YangDataChannelType dataType){
	if (m_dataMsg) 			
		m_dataMsg->receiveMsg(m_dataMsg->user, 0, data, nb);		
}

void YangIpcRtc::onIceStateChange(int32_t  uid,YangIceCandidateState iceState){
	switch(iceState){
	case YangIceSuccess:{
		break;
	}
	case YangIceFail:{
		yang_trace("\nYangIceFail remove uid==%d",uid);
		disconnectPeer();
		break;
	}
	default:break;

	}
}
void YangIpcRtc::onConnectionStateChange(int32_t  uid,YangRtcConnectionState connectionState){
	
	switch(connectionState){
	case Yang_Conn_State_Connecting:{
		break;
	}
	case Yang_Conn_State_Connected:{
		m_connState = connectionState;
		break;
	}
	case Yang_Conn_State_Disconnected:{
		m_connState = connectionState;
		break;
	}
	case Yang_Conn_State_Failed:{
		yang_trace("\nYang_Conn_State_Failed remove uid==%d",uid);
		disconnectPeer();
		break;
	}
	case Yang_Conn_State_Closed:{
		break;
	}
	default:break;
	}
}

void YangIpcRtc::onIceCandidate(int32_t  uid,char* sdp){
	
	if(m_dataCallback){
		m_dataCallback->addIceCandidate(m_dataCallback->user,uid,sdp);
	}
}

void YangIpcRtc::onIceGatheringState(int32_t  uid,YangIceGatheringState gatherState){

}

void YangIpcRtc::setMediaConfig(int32_t  puid,YangAudioParam* audio,YangVideoParam* video){

}


void YangIpcRtc::sendRequest(int32_t  puid,uint32_t  ssrc,YangRequestType req){
	
	if (req == Yang_Req_Sendkeyframe && m_videoEncoder) {
		m_videoEncoder->sendMsgToEncoder(m_videoEncoder->session, req);
	}
}

void YangIpcRtc::sslCloseAlert(int32_t  uid) {
	m_connState = Yang_Conn_State_Closed;
}

YangIpcRtc::YangIpcRtc(YangAVInfo *avinfo,YangIpcDataCallback* dataCallback, YangIpcDataMsg* dataMsg){
	m_dataCallback = dataCallback;
	m_avinfo = avinfo;
	m_dataMsg= dataMsg;
	m_videoEncoder = NULL;

	m_isStart = yangfalse;
	m_isLoop = yangfalse;
	m_isConnected=yangfalse;
	m_inited = yangfalse;
	m_enablePacer = avinfo->rtc.enablePacer;
	m_connState = Yang_Conn_State_New;

	m_encoderCallback.session=this;
	m_encoderCallback.onAudioData=yang_onAudioData;
	m_encoderCallback.onVideoData=yang_onVideoData;

	m_encoderEnable.session=this;
	m_encoderEnable.enable= yang_ipc_rtc_enable;
	
	yang_mutex_init(&m_mutex);

	yang_memset(&m_param,0,sizeof(YangPeerInfo));
	yang_peerConn_initPeerParam(&m_param);
	yang_memcpy(&m_param.rtc, &m_avinfo->rtc, sizeof(YangRtcInfo));
	m_param.isControlled = yangtrue;
	
	m_peer = NULL;
	m_pacer = NULL;

}

YangIpcRtc::~YangIpcRtc(){
	m_isLoop = yangfalse;
	while (m_isStart) {
			yang_usleep(1000);
	}

	yang_delete(m_peer);
	yang_delete(m_pacer);
	yang_thread_mutex_destroy(&m_mutex);
}

