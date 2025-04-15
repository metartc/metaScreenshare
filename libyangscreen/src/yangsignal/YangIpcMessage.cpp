//
// Copyright (c) 2019-2024 yanggaofeng
//
#include "YangIpcMessage.h"
#include "YangIpcMessageParser.h"

yang_vector_impl2(YangIpcRequest)

static void yang_ipcmessage_putMessage(YangIpcMessageSession *session,  YangIpcRequest* request) {

	if (!session->isLoop)
		return;

	yang_thread_mutex_lock(&session->mutex);
	session->messages.insert(&session->messages.vec, request);
	yang_thread_mutex_unlock(&session->mutex);

}


 int32_t yang_ipcmessage_sendmsg(YangIpcMessageSession *session,char* topic,YangIpcRequest* request){

	if(session==NULL||!session->isConnected||session->mqtt==NULL) return 1;

	if(request->cid[0]==0){
		char* cid=session->mqtt->getClientId(session->mqtt->session);
		if(cid) yang_memcpy(request->cid,cid,Yang_Cid_Length);
	}

	return yang_ipcmsg_send(session->mqtt,topic,request);
}


static void yang_ipcmessage_mqtt_receive(void* psession,char* topic,char* msg){

	if(psession==NULL || msg==NULL) return;
	YangIpcMessageSession *session = (YangIpcMessageSession*) psession;

	YangIpcRequest *request = (YangIpcRequest*) yang_calloc(sizeof(YangIpcRequest),	1);
	if(yang_ipcmsg_parse(msg,request)!=Yang_Ok){
		yang_free(request);
		return;
	}
	
	yang_ipcmessage_putMessage(session,request);
}

static void yang_ipcmessage_mqtt_connect_error(void *psession,int32_t errCode){
	if(psession==NULL) return;
	YangIpcMessageSession *session = (YangIpcMessageSession*) psession;
	session->isConnected=yangfalse;
	yang_error("yang_ipcmessage_mqtt connect error");
}

void* yang_run_ipcmessage_thread(void *obj) {
	YangIpcMessageSession *session = (YangIpcMessageSession*) obj;

	session->isStart = yangtrue;
	session->isLoop = yangtrue;

	while (session->isLoop) {
	
		while (session->messages.vec.vsize > 0) {

			YangIpcRequest *request = session->messages.vec.payload[0];
			
			if(session->callback.handleMessage)
				session->callback.handleMessage(session,request);
		
			yang_free(request->url);
			yang_free(request->data);

			yang_free(session->messages.vec.payload[0]);
			yang_thread_mutex_lock(&session->mutex);
			session->messages.remove(&session->messages.vec, 0);
			yang_thread_mutex_unlock(&session->mutex);

		}
		yang_usleep(100*1000);
	}

	session->isStart = yangfalse;
	return NULL;
}


static int32_t yang_ipcmessage_start(YangIpcMessageSession *session,char* subscribeTopic) {
	int32_t err=Yang_Ok;
	if(session==NULL) return 1;

	char* username=yang_strlen(session->mqttInfo->mqttUserName)==0?NULL:session->mqttInfo->mqttUserName;
	char* password=yang_strlen(session->mqttInfo->mqttPassword)==0?NULL:session->mqttInfo->mqttPassword;
	char* remoteIp=session->mqttInfo->mqttServerIP;
	int32_t port=session->mqttInfo->mqttPort;
	session->isMqttLoop=yangtrue;

	if(session->topic==NULL){
		session->topic=(char*)yang_calloc(1,yang_strlen(subscribeTopic)+1);
		yang_strcpy(session->topic,subscribeTopic);
	}
	err = session->mqtt->connect(session->mqtt->session, 16 * 1024, 16 * 1024, remoteIp, port,username, password);
	if ( err!= Yang_Ok)
	{
		yang_error("mqtt connect fail(%d)",err);
		return err;
	}
	session->isConnected = yangtrue;
	session->mqtt->subscribe(session->mqtt->session,session->topic);

	if ((err=yang_thread_create(&session->threadId, 0, yang_run_ipcmessage_thread,
			session)) != Yang_Ok) {
		yang_error("YangThread::start could not start thread");
		err=ERROR_THREAD;
	}
	return err;

}

static void yang_ipcmessage_stop(YangIpcMessageSession *session) {
	if(session==NULL) return;

	session->isLoop = yangfalse;
	session->isMqttLoop=yangfalse;
}



static int32_t yang_ipcmessage_publish(YangIpcMessageSession* session,char* topic,char* msg,int32_t msgLen){
	if(session==NULL || topic==NULL) return ERROR_MQTT_SERVER;

	if(session->isConnected&&session->mqtt&&session->mqtt->publish) {

		return session->mqtt->publish(session->mqtt->session,topic,msg,msgLen);
	}
	return ERROR_MQTT_SERVER;
}

int32_t yang_create_ipcmessage(YangIpcMessage *message,YangIpcMessageCallback* msgcallback,YangAVInfo* avinfo,yangbool enableTls) {
	if(message==NULL || avinfo==NULL ) return ERROR_MQTT_SERVER;
	YangIpcMessageSession *session = &message->session;
	session->isLoop = yangfalse;
	session->isStart = yangfalse;
	session->isMqttLoop=yangfalse;
	session->isMqttStart=yangfalse;
	session->isConnected=yangfalse;

	session->topic=NULL;
	session->avinfo=avinfo;
	session->mqttInfo=avinfo->mqtt;
	session->uidSeq=10;

	session->callback.user=msgcallback->user;
	session->callback.handleMessage=msgcallback->handleMessage;

	yang_mutex_init(&session->mutex);

	yang_create_YangIpcRequestVector2(&session->messages);

	YangMqttCallback callback;
	callback.session=session;
	callback.mqtt_receive=yang_ipcmessage_mqtt_receive;
	callback.mqtt_connect_error=yang_ipcmessage_mqtt_connect_error;

	session->mqtt = (YangMqtt*)yang_calloc(sizeof(YangMqtt), 1);
	yang_create_mqtt(session->mqtt,avinfo,&callback,enableTls);



	if(session->mqtt->setReconnectParam)
		session->mqtt->setReconnectParam(session->mqtt->session,avinfo->mqtt->maxReconnectTimes,avinfo->mqtt->reconnectIntervalTime);


	message->start = yang_ipcmessage_start;
	message->stop = yang_ipcmessage_stop;
	message->putMessage = yang_ipcmessage_putMessage;

	message->publish = yang_ipcmessage_publish;

	return Yang_Ok;
}

void yang_destroy_ipcmessage(YangIpcMessage *message) {
	if(message==NULL) return;
	YangIpcMessageSession *session = &message->session;

	if(session->mqtt){
		yang_destroy_mqtt(session->mqtt);
		yang_free(session->mqtt);
	}

	yang_ipcmessage_stop(session);

	if (session->isStart||session->isMqttStart) {

		while (session->isStart||session->isMqttStart) {
			yang_usleep(1000);
		}
	}

	while (session->messages.vec.vsize > 0) {
		YangIpcRequest *request = session->messages.vec.payload[0];
		session->callback.handleMessage(session,request);
		yang_free(session->messages.vec.payload[0]);

		session->messages.remove(&session->messages.vec, 0);

	}

	yang_thread_mutex_destroy(&session->mutex);

	yang_destroy_YangIpcRequestVector2(&session->messages);
	yang_free(session->topic);

}
