//
// Copyright (c) 2019-2024 yanggaofeng
//

#ifndef INCLUDE_YANGSIGNAL_YANGIPCMESSAGESERVER_H_
#define INCLUDE_YANGSIGNAL_YANGIPCMESSAGESERVER_H_
#include <yangutil/yangtype.h>
#include <yangipc/YangIpcConfig.h>
#include <yangsignal/YangIpcMessageDef.h>

typedef struct{
	void* session;
	int32_t  (*start)(void* session,char* subscribeTopic);
	void (*stop)(void* session);
	int32_t  (*publish)(void* session,char* topic,char* msg,int32_t  msgLen);
	int32_t  (*addCandidate)(void* session,int32_t  uid,char* candidateStr);
	int32_t  (*checkLive)(void* session);
}YangIpcMessageServer;



int32_t  yang_create_ipcmessageServer(YangIpcMessageServer *message,YangIpcHandle* receive,YangIpcDataMsg* dataMsg,YangAVInfo* avinfo);
void yang_destroy_ipcmessageServer(YangIpcMessageServer* message);




#endif /* INCLUDE_YANGSIGNAL_YANGIPCMESSAGESERVER_H_ */
