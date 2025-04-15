
//
// Copyright (c) 2019-2025 yanggaofeng
//
#ifndef SRC_IPC_YangP2pRtc_H_
#define SRC_IPC_YangP2pRtc_H_
#include <yangutil/yangavinfo.h>
#include <yangrtc/YangRtcPacer.h>

#include <yangipc/YangIpcConfig.h>


#include <yangvideo/YangNalu.h>

#include <yangutil/sys/YangVector.h>
#include <yangutil/buffer2/YangFrameBuffer.h>


class YangIpcRtc:public YangCallbackReceive,public YangCallbackIce,public YangCallbackRtc,public YangCallbackSslAlert{
public:
	YangIpcRtc(YangAVInfo *avinfo,YangIpcDataCallback* dataCallback, YangIpcDataMsg* dataMsg);
	virtual ~YangIpcRtc();
	void init();
	void start();
	void stop();

	int32_t connect(char* sdp,char* answer,char* remoteIp,int32_t localPort,int32_t uid);

	int32_t addCandidate(int32_t uid,char* candidate);
	int32_t setLocalDescription(int32_t uid,char* sdp);

	int32_t disconnectPeer();

	void checkAlive();
	yangbool isAlive();
	int32_t publishMsg(YangFrame* msgFrame);


public:
	void receiveAudio(uint8_t* data,int32_t nb,uint64_t pts);
	//frametype:YANG_Frametype_I YANG_Frametype_P
	void receiveVideo(uint8_t* data,int32_t nb,uint64_t pts,int32_t frametype);
	void receiveMsg(uint8_t* data,int32_t nb,YangDataChannelType dataType);

	void onIceStateChange(int32_t  uid,YangIceCandidateState iceState);
	void onConnectionStateChange(int32_t  uid,YangRtcConnectionState connectionState);
	void onIceCandidate(int32_t  uid,char* sdp);
	void onIceGatheringState(int32_t  uid,YangIceGatheringState gatherState);

	void setMediaConfig(int32_t  puid,YangAudioParam* audio,YangVideoParam* video);
	void sendRequest(int32_t  puid,uint32_t  ssrc,YangRequestType req);

	void sslCloseAlert(int32_t  uid);
public:
	yangbool m_isStart;
	yangbool m_isLoop;

	yangbool m_inited;
	yangbool m_enablePacer;
	yangbool m_isConnected;

	YangRtcConnectionState m_connState;
	yang_thread_t m_threadId;

	yang_thread_mutex_t m_mutex;
	YangAVInfo* m_avinfo;
	YangIpcDataMsg* m_dataMsg;
	YangIpcDataCallback* m_dataCallback;

	YangRtcPacer* m_pacer;
	YangPeerConnection8* m_peer;
	YangEncoderVideo* m_videoEncoder;

	YangPeerInfo m_param;
	YangRtcCallback m_rtcCallback;
	YangCodecEnable m_encoderEnable;
	YangCodecCallback m_encoderCallback;

};



#endif /* SRC_YANGMEETING_INCLUDE_YangP2pRtc_H_ */
