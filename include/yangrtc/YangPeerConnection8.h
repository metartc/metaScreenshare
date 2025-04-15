/*
 * YangPeerconnection8.h
 *
 *  Created on: 2025年2月25日
 *      Author: yanggaofeng
 */

#ifndef SRC_YANGPEERCONNECTION8_H_
#define SRC_YANGPEERCONNECTION8_H_
#include <stdint.h>
#ifndef INCLUDE_YANGUTIL_YANGAVINFO_H_
#define INCLUDE_YANGUTIL_YANGAVINFO_H_

typedef  uint8_t yangbool;
#define yangtrue 1
#define yangfalse 0

#define YANG_Frametype_I 1
#define YANG_Frametype_P 0

typedef enum {
	Yang_IpFamilyType_IPV4,
	Yang_IpFamilyType_IPV6
} YangIpFamilyType;

typedef enum{
	YangTransportAll,
	YangTransportRelay
}YangIceTransportPolicy;

typedef enum {
	YangIceNew,
    YangIceSuccess,
    YangIceFail
} YangIceCandidateState;

typedef enum {
	YangIceGatherNew,
	YangIceGathering,
    YangIceGatherComplete
} YangIceGatheringState;

typedef enum {
    Yang_Conn_State_New,
    Yang_Conn_State_Connecting,
    Yang_Conn_State_Connected,
    Yang_Conn_State_Disconnected,
    Yang_Conn_State_Failed,
    Yang_Conn_State_Closed
} YangRtcConnectionState;
typedef enum YangAudioCodec{
	Yang_AED_AAC,
	Yang_AED_MP3,
	Yang_AED_SPEEX,
	Yang_AED_OPUS,
	Yang_AED_PCMA,
	Yang_AED_PCMU
}YangAudioCodec;

typedef enum{
	Yang_VED_H264,
	Yang_VED_H265
}YangVideoCodec;

typedef enum{
	YangMediaAudio,
	YangMediaVideo
}YangMediaTrack;

typedef enum{
	YangSendrecv,
	YangSendonly,
	YangRecvonly,
	YangInactive
}YangRtcDirection;

typedef struct {
	YangAudioCodec encode;
	int32_t  sample;
	int32_t  channel;
    int32_t  audioClock;
    int32_t  fec;
}YangAudioParam;

typedef struct  {
	YangVideoCodec encode;
	int32_t  videoClock;
}YangVideoParam;

typedef enum YangRequestType {
	Yang_Req_Sendkeyframe,
	Yang_Req_HighLostPacketRate,
	Yang_Req_LowLostPacketRate,
	Yang_Req_Connected,
	Yang_Req_Disconnected
}YangRequestType;

typedef struct{
	int32_t enableAudioFec;
	int32_t sample;
	int32_t channel;

	YangAudioCodec  audioEncoderType;
}YangPushAudioInfo;

typedef struct{
	int32_t width;
	int32_t height;
	int32_t fps;
	YangVideoCodec  videoEncoderType;
}YangPushVideoInfo;

typedef enum{
	YANG_DATA_CHANNEL_DCEP = 50,
	YANG_DATA_CHANNEL_STRING = 51,
	YANG_DATA_CHANNEL_BINARY = 53,
	YANG_DATA_CHANNEL_STRING_EMPTY = 56,
	YANG_DATA_CHANNEL_BINARY_EMPTY = 57
}YangDataChannelType;


typedef struct YangRtcInfo {
	yangbool enableNetEQ;
	yangbool enablePacer;
	yangbool enableFec;
	yangbool enableHttpServerSdp;

	int32_t  sessionTimeout;
	int32_t  iceCandidateType;
	YangIceTransportPolicy iceTransportPolicy;

	int32_t  iceServerPort;

	int32_t  rtcSocketProtocol;
	int32_t  turnSocketProtocol;

	int32_t  rtcPort;
	int32_t  rtcLocalPort;

	int32_t turnReqInterval;
	int32_t maxTurnWaitTime;

	int32_t maxPacerDelayTime;
	int32_t nackPktCount;

	char iceServerIP[64];
	char iceUserName[32];
	char icePassword[64];
}YangRtcInfo;

typedef struct {
	yangbool enableHttps;
	yangbool isControlled;
	int16_t mediaServer;

	int32_t uid;
	int32_t userId;
	int32_t remotePort;
	YangIpFamilyType familyType;
	YangRtcDirection direction;

	YangRtcInfo rtc;
	YangPushAudioInfo pushAudio;
	YangPushVideoInfo pushVideo;
}YangPeerInfo;

#endif
#if _WIN32
#define YANG_EXPORT_API __declspec(dllexport)
#else
#define YANG_EXPORT_API
#endif

class YangCallbackReceive {
public:
	YangCallbackReceive(){};
	virtual ~YangCallbackReceive(){};
	virtual void receiveAudio(uint8_t* data,int32_t nb,uint64_t pts)=0;
	//frametype:YANG_Frametype_I YANG_Frametype_P
	virtual void receiveVideo(uint8_t* data,int32_t nb,uint64_t pts,int32_t frametype)=0;
	virtual void receiveMsg(uint8_t* data,int32_t nb,YangDataChannelType dataType)=0;
};

class YangCallbackIce{
public:
	YangCallbackIce(){};
	virtual ~YangCallbackIce(){};
	virtual void onIceStateChange(int32_t  uid,YangIceCandidateState iceState)=0;
	virtual void onConnectionStateChange(int32_t  uid,YangRtcConnectionState connectionState)=0;
	virtual void onIceCandidate(int32_t  uid,char* sdp)=0;
	virtual void onIceGatheringState(int32_t  uid,YangIceGatheringState gatherState)=0;
};

class YangCallbackRtc{
public:
	YangCallbackRtc(){};
	virtual ~YangCallbackRtc(){};
	virtual void setMediaConfig(int32_t  puid,YangAudioParam* audio,YangVideoParam* video)=0;
	virtual void sendRequest(int32_t  puid,uint32_t  ssrc,YangRequestType req)=0;
};

class YangCallbackSslAlert {
public:
	YangCallbackSslAlert() {};
	virtual ~YangCallbackSslAlert() {};
	virtual void sslCloseAlert(int32_t  uid) = 0;
};


class YangPeerConnection8 {
public:
	YangPeerConnection8() {};
	virtual ~YangPeerConnection8() {};
	virtual int32_t  addAudioTrack(YangAudioCodec codec) = 0;
	virtual int32_t  addVideoTrack(YangVideoCodec codec) = 0;

	virtual int32_t  addTransceiver(YangMediaTrack media, YangRtcDirection direction) = 0;

	virtual int32_t  createOffer(char** psdp) = 0;
	virtual int32_t  createAnswer(char* answer) = 0;
	virtual int32_t  createHttpAnswer(char* answer) = 0;
	virtual int32_t  createDataChannel() = 0;

	virtual int32_t  generateCertificate() = 0;

	virtual int32_t  setLocalDescription(char* sdp) = 0;
	virtual int32_t  setRemoteDescription(char* sdp) = 0;

	virtual int32_t  connectSfuServer(int32_t mediaServer) = 0;//srs zlm
	virtual int32_t  connectWhipServer(char* url) = 0;

	virtual int32_t  close() = 0;

	virtual int32_t  isAlive() = 0;
	virtual int32_t  isConnected() = 0;

	virtual int32_t  on_message(char* data, int32_t nbbyte, YangDataChannelType dtType) = 0;

	virtual int32_t  addIceCandidate(char* candidateStr) = 0;
	virtual int32_t  sendRequestPli() = 0;

};

 void yang_peerConn_initPeerParam(YangPeerInfo* peerInfo);
 YangPeerConnection8* yang_create_peerConnection8(YangPeerInfo* peerInfo, YangCallbackReceive* receive, YangCallbackIce* ice, YangCallbackRtc* rtc, YangCallbackSslAlert* sslAlert);

#endif /* SRC_YANGPEERCONNECTION8_H_ */
