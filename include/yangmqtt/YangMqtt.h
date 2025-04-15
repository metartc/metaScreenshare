//
// Copyright (c) 2019-2023 yanggaofeng
//

#ifndef INCLUDE_YANGMQTT_YANGMQTT_H_
#define INCLUDE_YANGMQTT_YANGMQTT_H_
#include <yangutil/yangtype.h>
#include <yangutil/yangavinfo.h>
#define Yang_Mqtt_ReconnectFail 1
typedef struct{
	void* session;
	void (*mqtt_receive)(void* session,char* topic,char* msg);
	void (*mqtt_connect_error)(void* session,int32_t  errCode);
}YangMqttCallback;

typedef struct{
	void* session;
	int32_t  (*connect)(void* session,int32_t  sendBufferSize,int32_t  recvBufferSize,char *remoteIp,int32_t  remotePort,char* userName,char* password);
	int32_t  (*reconnect)(void* psession);
	int32_t  (*stop)(void* psession);
	int32_t  (*close)(void* session);
	int32_t  (*publish)(void* session,char* topic,char* msg,int32_t  msgLen);
	int32_t  (*subscribe)(void* session,char* topic);
	int32_t  (*unsubscribe)(void* session,char* topic);
	char* 	(*getClientId)(void* session);
	void    (*setReconnectParam)(void* session,int32_t  maxReconnectTimes,int32_t  reconnectIntervalTime);
}YangMqtt;

#ifdef __cplusplus
extern "C"{
#endif
	int32_t  yang_create_mqtt(YangMqtt* mqtt,YangAVInfo* avinfo,YangMqttCallback* mqttcb,yangbool enableTls);
	void yang_destroy_mqtt(YangMqtt* mqtt);
#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_YANGMQTT_YANGMQTT_H_ */
