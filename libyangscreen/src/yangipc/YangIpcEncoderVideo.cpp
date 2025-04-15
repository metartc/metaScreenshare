//
// Copyright (c) 2019-2023 yanggaofeng
//
#include <yangipc/YangIpcEncoder.h>

#include <yangencoder/YangAudioEncoderHandle.h>
#include <yangencoder/YangVideoEncoderHandle2.h>

#include <yangipc/YangIpcVideoBuffer.h>

#include <yangutil/sys/YangLog.h>

#include <yangencoder/YangEncoderFactory.h>
#include <yangipc/YangIpcConfig.h>

#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangTime.h>

#include <yangcapture/win/YangScreenWin.h>

typedef struct {
	yangbool isStart;
	yangbool isLoop;
	yangbool sendKeyframe;
	yangbool hasConnected;
	yang_thread_t threadId;
	YangFrameBuffer *out_videoBuffer;
	YangScreenWin* screen;
	YangAVInfo *avinfo;
	YangCodecCallback callback;
	YangCodecEnable codecEnable;
} YangP2pEncoderVideo;




class YangIpcCaptureCallback:public YangEncoderCallback{
public:
	YangP2pEncoderVideo* m_session=NULL;
	YangIpcCaptureCallback(YangP2pEncoderVideo* session){
		m_session=session;
	};
	~YangIpcCaptureCallback(){

	};
	void onVideoData(YangFrame* pframe){
		m_session->callback.onVideoData(m_session->callback.session,pframe);
	}
	void onAudioData(YangFrame* pframe){

	}
};


YangP2pEncoderVideo* g_yang_p2p_encoderVideo = NULL;


YangVideoBuffer* yang_get_ipc_videoBuffer() {
	if (g_yang_p2p_encoderVideo && g_yang_p2p_encoderVideo->screen)
		return g_yang_p2p_encoderVideo->screen->getOutVideoBuffer();
	return NULL;
}

void yang_p2pEncoderVideo_putstream(void *session, YangFrame *frame) {
	if (session == NULL || frame == NULL)
		return;

}

void* yang_p2pEncoderVideo_thread(void *obj) {
	YangP2pEncoderVideo *session = (YangP2pEncoderVideo*) obj;
	YangIpcCaptureCallback callback(session);
	YangEncoderFactory ydf;
	YangVideoEncoder *encoder =NULL;

	session->isStart = yangtrue;
	session->isLoop = yangtrue;
	if(session->callback.session==NULL){
		session->callback.session = session;
		session->callback.onVideoData = yang_p2pEncoderVideo_putstream;
	}

	YangFrame videoFrame = { 0 };
	int32_t videoLen = session->avinfo->video.width	* session->avinfo->video.height * 3 / 2;
	int64_t baseTs = 0;
	
	YangRtcEncoderMessage rtcMsg;
	rtcMsg.request=Yang_Req_Sendkeyframe;
	uint8_t* buffer=NULL;

	while (session->isLoop) {
		buffer = session->screen->getFrame();
	
		if (buffer == NULL) {
			yang_usleep(1000 * 5);
			continue;
		}
		if (encoder == NULL) {
			encoder = ydf.createVideoEncoder(&session->avinfo->video);
			YangContext context;
			memcpy(&context.avinfo, session->avinfo, sizeof(YangAVInfo));
			encoder->init(&context, &session->avinfo->video);
		}
		
		if(session->codecEnable.session&&session->codecEnable.enable){

			if(!session->codecEnable.enable(session->codecEnable.session)){
				baseTs=0;
				yang_usleep(1000 * 5);
				continue;
			}
		}
		
		if (baseTs == 0)
			baseTs = yang_get_system_time();
		videoFrame.payload = buffer;
		videoFrame.nb = videoLen;
		videoFrame.dts = videoFrame.pts = yang_get_system_time() - baseTs;
		videoFrame.frametype = 0;
		if(session->sendKeyframe){
			encoder->sendMsgToEncoder(&rtcMsg);
			session->sendKeyframe=yangfalse;
		}
	
		encoder->encode( &videoFrame,&callback);
	}
	yang_trace("yangencoder_close_thread \n");

	yang_delete(encoder);
	session->isStart = yangfalse;
	return NULL;
}

void yang_p2pEncoderVideo_sendMsgToEncoder(void *psession,
		YangRequestType request) {
	if (psession == NULL)
		return;
	YangP2pEncoderVideo* session=(YangP2pEncoderVideo*)psession;
	if (request == Yang_Req_Sendkeyframe) {
		session->sendKeyframe=yangtrue;
	} else if( request == Yang_Req_Connected){

	}
}

static int32_t yang_p2pEncoderVideo_start(void *psession) {
	if (psession == NULL)
		return 1;
	YangP2pEncoderVideo *session = (YangP2pEncoderVideo*) psession;

	if (session->isStart)
		return Yang_Ok;
	if(session->screen)
		session->screen->start();
	if (yang_thread_create(&session->threadId, 0, yang_p2pEncoderVideo_thread,
			session)) {
		yang_error("YangThread::start could not start thread");
	}
	return Yang_Ok;
}

static void yang_p2pEncoderVideo_stop(void *psession) {
	YangP2pEncoderVideo *session=NULL;
	if (psession == NULL)
		return;
	session = (YangP2pEncoderVideo*) psession;
	session->isLoop = yangfalse;
}

static int32_t yang_p2pEncoderVideo_init(void *psession,YangCodecEnable *codecEnable,YangCodecCallback *callback) {
	YangP2pEncoderVideo *session = (YangP2pEncoderVideo*) psession;
	if (psession == NULL)
		return 1;
	if (session->screen == NULL) {
		//session->screen = new YangCaputreScreen2(session->avinfo);
		session->screen = new YangScreenWin(session->avinfo);
		session->screen->init();
		
	}

	if(callback){
		session->callback.session=callback->session;
		session->callback.onAudioData=callback->onAudioData;
		session->callback.onVideoData=callback->onVideoData;
	}

	if(codecEnable){
		session->codecEnable.session=codecEnable->session;
		session->codecEnable.enable=codecEnable->enable;
	}

	return Yang_Ok;
}

int32_t yang_create_videoEncoder(YangEncoderVideo *encoder,
		YangAVInfo *avinfo) {
	if (encoder == NULL || avinfo == NULL)
		return 1;
	YangP2pEncoderVideo *session = (YangP2pEncoderVideo*) yang_calloc(
			sizeof(YangP2pEncoderVideo), 1);
	encoder->session = session;
	g_yang_p2p_encoderVideo = session;
	session->avinfo = avinfo;
	session->screen = NULL;
	session->out_videoBuffer = NULL;
	session->callback.session = NULL;

	session->codecEnable.session = NULL;

	session->isLoop = yangfalse;
	session->isStart = yangfalse;
	session->sendKeyframe=yangfalse;

	encoder->init = yang_p2pEncoderVideo_init;
	encoder->start = yang_p2pEncoderVideo_start;
	encoder->stop = yang_p2pEncoderVideo_stop;
	encoder->sendMsgToEncoder = yang_p2pEncoderVideo_sendMsgToEncoder;

	return Yang_Ok;
}

void yang_destroy_videoEncoder(YangEncoderVideo *encoder) {
	if (encoder == NULL || encoder->session == NULL)
		return;
	g_yang_p2p_encoderVideo = NULL;
	YangP2pEncoderVideo *session = (YangP2pEncoderVideo*) encoder->session;
	session->isLoop = yangfalse;
	if (session->isStart) {
		while (session->isStart) {
			yang_usleep(1000);
		}
	}
	if (session->screen)
		session->screen->stop();
	yang_delete(session->screen);
	session->out_videoBuffer = NULL;

}



