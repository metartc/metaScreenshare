//
// Copyright (c) 2019-2023 yanggaofeng
//

#ifndef INCLUDE_YANGIPC_YANGIPCCONFIG_H_
#define INCLUDE_YANGIPC_YANGIPCCONFIG_H_
#include <yangutil/yangavinfo.h>
#include <yangipc/YangIpcEncoder.h>



typedef struct{
		void* user;
		void (*receiveMsg)(void* user,int32_t uid,uint8_t* data,int32_t len);
}YangIpcDataMsg;

typedef struct{
	void* user;
	int32_t (*addIceCandidate)(void* user,int32_t uid,char* candidateStr);
	void (*onConnectionStateChange)(void* user, int32_t uid,YangRtcConnectionState connectionState);
}YangIpcDataCallback;


typedef struct{
	YangIpcServerType ipcServerType;
	YangAVInfo* avinfo;

	YangEncoderVideo* videoEncoder;
	YangIpcDataMsg* dataMsg;
	//YangIpcPlayCallback* playCallback;
	YangCodecCallback *encoderCallback;
	YangIpcDataCallback* dataCallback;
}YangIpcConfig;




#ifdef __cplusplus
extern "C"{
#endif
yangbool yang_ipc_encoder_enable();
uint32_t yang_ipc_getConnectionCount();
void yang_ipc_initAVInfoFromFile(YangAVInfo* avinfo,char* filename);
void yang_ipc_initAVInfo(YangAVInfo* avinfo);
void yang_ipc_closeLogFile();

#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_YANGIPC_YANGIPCCONFIG_H_ */
