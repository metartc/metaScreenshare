
//
// Copyright (c) 2019-2025 yanggaofeng
//
#ifndef SRC_IPC_YangIPCMessage_H_
#define SRC_IPC_YangIPCMessage_H_

#include <yangsignal/YangIpcMessageDef.h>
#include <yangutil/yangavinfo.h>
#include <yangutil/sys/YangCString.h>
#include <yangutil/sys/YangThread.h>
#include <yangutil/sys/YangVector.h>

#include <yangmqtt/YangMqtt.h>

yang_vector_declare2(YangIpcRequest)

typedef struct{
	void* user;
	void (*handleMessage)(void *session,YangIpcRequest *mss);
}YangIpcMessageCallback;

typedef struct {
	yangbool isStart;
	yangbool isLoop;
	yangbool isMqttStart;
	yangbool isMqttLoop;
	yangbool isConnected;

	uint32_t uidSeq;

	yang_thread_t threadId;
	yang_thread_t mqttThreadId;
	yang_thread_mutex_t mutex;

	char* topic;
	YangAVInfo* avinfo;
	YangMqttInfo* mqttInfo;
	YangMqtt* mqtt;
	YangIpcMessageCallback callback;
	YangIpcRequestVector2 messages;
} YangIpcMessageSession;

typedef struct{
	YangIpcMessageSession session;

	void (*putMessage)(YangIpcMessageSession* session,YangIpcRequest* request);
	int32_t (*start)(YangIpcMessageSession* session,char* subscribeTopic);
	void (*stop)(YangIpcMessageSession* session);
	int32_t (*publish)(YangIpcMessageSession* session,char* topic,char* msg,int32_t msgLen);

}YangIpcMessage;


int32_t yang_ipcmessage_sendmsg(YangIpcMessageSession *session,char* topic,YangIpcRequest* request);
int32_t yang_create_ipcmessage(YangIpcMessage *message,YangIpcMessageCallback* callback,YangAVInfo* avinfo, yangbool enableTls);
void yang_destroy_ipcmessage(YangIpcMessage* message);


#endif /* SRC_YANGSIGNAL_YANGMESSAGEHANDLE_H_ */
