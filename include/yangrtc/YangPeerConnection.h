//
// Copyright (c) 2019-2024 yanggaofeng
//

#ifndef INCLUDE_YANGRTC_YANGPEERCONNECTION_H_
#define INCLUDE_YANGRTC_YANGPEERCONNECTION_H_

#include <yangrtc/YangPushData.h>

typedef struct{
	void* conn;
	YangPeerInfo peerInfo;
	YangStreamConfig streamconfig;
}YangPeer;

typedef struct {
	YangPeer peer;

	int32_t  (*addAudioTrack)(YangPeer* peer,YangAudioCodec codec);
	int32_t  (*addVideoTrack)(YangPeer* peer,YangVideoCodec codec);
	int32_t  (*addTransceiver)(YangPeer* peer,YangMediaTrack media,YangRtcDirection direction);

	int32_t  (*createOffer)(YangPeer* peer, char **psdp);
	int32_t  (*createAnswer)(YangPeer* peer,char* answer);
	int32_t  (*createHttpAnswer)(YangPeer* peer,char* answer);
	int32_t  (*createDataChannel)(YangPeer* peer);

	int32_t  (*generateCertificate)(YangPeer* peer);
	int32_t  (*setCertificateFile)(YangPeer* peer,char* pkeyfile,char* certfile);

	int32_t  (*setLocalDescription)(YangPeer* peer,char* sdp);
	int32_t  (*setRemoteDescription)(YangPeer* peer,char* sdp);

	int32_t  (*connectSfuServer)(YangPeer* peer,int32_t mediaServer);//srs zlm
	int32_t  (*connectWhipWhepServer)(YangPeer* peer,char* url);

	int32_t  (*close)(YangPeer* peer);

	yangbool (*isAlive)(YangPeer* peer);
	yangbool (*isConnected)(YangPeer* peer);

	int32_t  (*on_audio)(YangPeer* peer,YangPushData *audioData);
	int32_t  (*on_video)(YangPeer* peer,YangPushData *videoData);
	int32_t  (*on_message)(YangPeer* peer,YangFrame* msgFrame);

	int32_t  (*addIceCandidate)(YangPeer* peer,char* candidateStr);
	int32_t  (*sendRequestPli)(YangPeer* peer);

	YangRtcConnectionState (*getConnectionState)(YangPeer* peer);
	YangIceCandidateType (*getIceCandidateType)(YangPeer* peer);

}YangPeerConnection;


#ifdef __cplusplus
extern "C"{
#endif
void yang_create_peerConnection(YangPeerConnection* peerconn);
void yang_destroy_peerConnection(YangPeerConnection* peerconn);

void yang_peerConn_initPeerInfo(YangPeerInfo* peerInfo);
void yang_avinfo_initPeerInfo(YangPeerInfo* peerInfo,YangAVInfo* avinfo);

int32_t  yang_p2p_getHttpSdp(YangIpFamilyType familyType,char* httpIp,int32_t  httpPort,char* localSdp,char* remoteSdp);
int32_t yang_p2p_getHttpSdp2(YangIpFamilyType family,char* httpIp,char* app,char* stream,int32_t httpPort,char* localSdp,
		char* remoteSdp);
#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_YANGRTC_YANGPEERCONNECTION_H_ */
