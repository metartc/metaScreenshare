//
// Copyright (c) 2019-2024 yanggaofeng
//

#include <yangsignal/YangIpcMessageDef.h>
#include <yangsignal/YangIpcMessageServer.h>

#include <yangutil/sys/YangIni.h>
#include <yangutil/sys/YangTime.h>

#include "YangIpcMessage.h"

#define Yang_LiveTime_Second 30


typedef struct{
	int32_t uid;
	uint64_t ts;
	char cid[Yang_Cid_Length];
}YangConnCid;

yang_vector_declare2(YangConnCid)

typedef struct {
	yang_thread_mutex_t msgLock;

	YangAVInfo* avinfo;
	YangIpcDataMsg dataMsg;
	YangIpcHandle handle;
	YangIpcMessage message;
	YangConnCidVector2 connCids;
} YangIpcMessageServerSession;

yang_vector_impl2(YangConnCid)

void yang_ipcmessage_removeConnCidByUid(YangIpcMessageServerSession *session,int32_t uid){
	YangConnCid* ccid=NULL;
	if(session==NULL) return;
	yang_thread_mutex_lock(&session->msgLock);
	for (int32_t i = 0; i < session->connCids.vec.vsize; i++) {
		ccid=session->connCids.vec.payload[i];
		if(ccid&&ccid->uid==uid){
			yang_free(session->connCids.vec.payload[i]);
			session->connCids.remove(&session->connCids.vec,i);
			break;
		}
	}
	yang_thread_mutex_unlock(&session->msgLock);
}

YangConnCid* yang_ipcmessage_getConnCidByUid(YangIpcMessageServerSession *session,int32_t uid){
	YangConnCid* ccid=NULL;
	if(session==NULL) return NULL;

	for (int32_t i = 0; i < session->connCids.vec.vsize; i++) {
		ccid=session->connCids.vec.payload[i];
		if(ccid&&ccid->uid==uid) return ccid;
	}
	return NULL;
}

YangConnCid* yang_ipcmessage_getConnCidByCid(YangIpcMessageServerSession *session,char* cid){
	YangConnCid* ccid=NULL;
	if(session==NULL||cid==NULL) return NULL;

	for (int32_t i = 0; i < session->connCids.vec.vsize; i++) {
		ccid=session->connCids.vec.payload[i];
		if(ccid&&yang_memcmp(ccid->cid,cid,Yang_Cid_Length)==0) return ccid;
	}
	return NULL;
}

int32_t yang_ipcmessage_answer(YangIpcMessageSession *session,
		YangIpcRequest *mss) {
	int32_t err = Yang_Ok;
	char *answer = NULL;
	char *sdpstr = NULL;
	char *filename=NULL;

	YangIpcMessageServerSession* serverSession=NULL;
	YangIpcRequest request;
	yang_memset(&request,0,sizeof(YangIpcRequest));

	if (session == NULL || mss == NULL)
		return ERROR_MQTT_SERVER;

	answer = (char*) yang_calloc(1024 * 8, 1);
	sdpstr = mss->data;

	serverSession=(YangIpcMessageServerSession*)session->callback.user;

	if (serverSession&&serverSession->handle.connect) {
		
		if((err=serverSession->handle.connect(sdpstr, yang_strlen(sdpstr) + 1, answer, (char*)"",
						mss->uid, serverSession->handle.user))!=Yang_Ok){
			goto fail;
		}

	} else {
		yang_error("mqtt server publish error,session->receive is null");
		err = ERROR_MQTT_SERVER;
		goto cleanup;
	}

	request.connectType=YangM_ConnectType_P2p;
	request.requestType=YangIpcRequestAnswer;
	request.uid=mss->uid;
	request.data=answer;
	yang_memcpy(request.cid,mss->cid,sizeof(request.cid));


    if (yang_ipcmessage_sendmsg(session,request.cid[0]==0?(char*)Yang_Topic_Client:request.cid, &request) != Yang_Ok) {
		yang_error("mqtt server answer error");
		err = ERROR_MQTT_SERVER;
		goto cleanup;
	}

	
	if (serverSession&&serverSession->handle.setLocalDescription) {
		serverSession->handle.setLocalDescription( answer, mss->uid, serverSession->handle.user);
	}

	cleanup:
	yang_free(filename);
	yang_free(answer);
	return err;
	fail:
	request.connectType=YangM_ConnectType_P2p;
	request.requestType=YangIpcRequestHasConnected;
	request.uid=mss->uid;
	yang_memcpy(request.cid,mss->cid,sizeof(request.cid));
	yang_ipcmessage_sendmsg(session,request.cid[0]==0?(char*)Yang_Topic_Client:request.cid, &request);
	err=1;
	goto cleanup;
	return err;

}

int32_t yang_ipcmessage_candidate(YangIpcMessageSession *session,
		YangIpcRequest *mss) {
	YangIpcMessageServerSession* serverSession;

	if (session == NULL || mss == NULL)
		return ERROR_MQTT_SERVER;

	serverSession = (YangIpcMessageServerSession*)session->callback.user;
	if (serverSession&&serverSession->handle.addCandidate)
		return serverSession->handle.addCandidate(mss->uid,mss->data, serverSession->handle.user);

	return Yang_Ok;
}

int32_t yang_ipcmessage_handleMessage_ping(
		YangIpcMessageSession *session, YangIpcRequest *mss) {
	YangIpcMessageServerSession* serverSession=(YangIpcMessageServerSession*)session->callback.user;

	YangConnCid *connCid = yang_ipcmessage_getConnCidByCid(serverSession,mss->cid);

	if (connCid)
		connCid->ts=yang_get_system_time();


	return Yang_Ok;
}

int32_t yang_ipcmessage_handleMessage_verify(
		YangIpcMessageSession *session, YangIpcRequest *mss) {
	int32_t err=Yang_Ok;

	YangIpcMessageServerSession* serverSession=(YangIpcMessageServerSession*)session->callback.user;
	YangConnCid *connCid = yang_ipcmessage_getConnCidByCid(serverSession,mss->cid);
	YangIpcRequest request;
	yang_memset(&request,0,sizeof(YangIpcRequest));
	if (connCid == NULL) {
		request.connectType = mss->connectType;
		request.requestType = YangIpcRequestNotLogin;
		request.uid=mss->uid;
		yang_memcpy(request.cid,mss->cid,sizeof(request.cid));

		err = yang_ipcmessage_sendmsg(session, request.cid[0]==0?(char*)Yang_Topic_Client:request.cid,&request);
		if (err != Yang_Ok)
			return yang_error_wrap(err, "mqtt server verify error");
		return err;
	}


	return Yang_Ok;
}

void yang_ipcmessage_handleMessage_p2p(YangIpcMessageSession *session,
		YangIpcRequest *mss) {
	
	switch (mss->requestType) {
	case YangIpcRequestConnect: {
		break;
	}
	case YangIpcRequestConnectSdp: {
		yang_ipcmessage_answer(session, mss);
		break;
	}
	case YangIpcRequestAnswer: {

		break;
	}
	case YangIpcRequestCandidate: {
		yang_ipcmessage_candidate(session,mss);
		break;
	}
	case YangIpcRequestClose: {
		break;
	}
	case YangIpcRequestPing: {
		yang_ipcmessage_handleMessage_ping(session,mss);
		break;
	}
	default:
		break;
	}
}


int32_t yang_ipcmessage_handleMessage_close(
		YangIpcMessageSession *session, YangIpcRequest *mss) {
	YangIpcMessageServerSession* serverSession=(YangIpcMessageServerSession*)session->callback.user;

	if (serverSession&&serverSession->handle.close)
		serverSession->handle.close(mss->uid, serverSession->handle.user);

	return Yang_Ok;
}

int32_t yang_ipcmessage_handleMessage_login(
		YangIpcMessageSession *session, YangIpcRequest *mss) {
	int32_t err = Yang_Ok;
	YangConnCid *connCid = NULL;
	YangIpcMessageServerSession *serverSession =
			(YangIpcMessageServerSession*) session->callback.user;
	YangIpcRequest request ;
	yang_memset(&request,0,sizeof(YangIpcRequest));
	request.connectType = mss->connectType;
	request.requestType = YangIpcRequestLoginResponse;
	request.uid = session->uidSeq++;

	yang_memcpy(request.cid, mss->cid, sizeof(request.cid));

	yang_thread_mutex_lock(&serverSession->msgLock);

	connCid = yang_ipcmessage_getConnCidByCid(serverSession,mss->cid);

	if (connCid == NULL) {
		connCid = (YangConnCid*) yang_calloc(sizeof(YangConnCid), 1);
		yang_memcpy(connCid->cid, request.cid, sizeof(request.cid));
		connCid->ts = yang_get_system_time();

		serverSession->connCids.insert(&serverSession->connCids.vec, connCid);

	}

	if(serverSession->handle.close)
			serverSession->handle.close(connCid->uid,serverSession->handle.user);

	connCid->uid = request.uid;
	yang_thread_mutex_unlock(&serverSession->msgLock);

	if ((err = yang_ipcmessage_sendmsg(session,
			request.cid[0] == 0 ? (char*)Yang_Topic_Client : request.cid, &request))
			!= Yang_Ok)
		return yang_error_wrap(err, "mqtt server login error");

	return err;
}

int32_t yang_ipcmessage_handleMessage_data(
		YangIpcMessageSession *session, YangIpcRequest *mss) {
	int32_t err = Yang_Ok;
	YangIpcMessageServerSession* serverSession=(YangIpcMessageServerSession*)session->callback.user;
	if (serverSession&&serverSession->dataMsg.receiveMsg && mss->data) {
		serverSession->dataMsg.receiveMsg(serverSession->dataMsg.user, mss->uid,
				(uint8_t*) mss->data, yang_strlen(mss->data) + 1);
	}
	return err;

}

void yang_ipcmessage_handleMessage(void* psession,
		YangIpcRequest *mss) {
	YangIpcMessageSession *session=(YangIpcMessageSession *)psession;
	if(psession==NULL) return;

	if (mss->requestType == YangIpcRequestLogin) {
		yang_ipcmessage_handleMessage_login(session, mss);
		return;
	}

	if(yang_ipcmessage_handleMessage_verify(session,mss)!=Yang_Ok){
		return;
	}

	if (mss->requestType == YangIpcRequestClose) {
		yang_ipcmessage_handleMessage_close(session, mss);
		return;
	}

	switch (mss->connectType) {
	case YangM_ConnectType_P2p: {
		yang_ipcmessage_handleMessage_p2p(session, mss);
		break;
	}
	case YangM_ConnectType_Datachannel: {
		yang_ipcmessage_handleMessage_data(session, mss);
		break;
	}
	default:
		break;

	}

}

int32_t yang_ipcmsg_start(void *psession, char* subscribeTopic) {
	YangIpcMessageServerSession *session =	(YangIpcMessageServerSession*) psession;
	if (psession == NULL)
		return 1;
	
	return session->message.start(&session->message.session,subscribeTopic);
}

void yang_ipcmsg_stop(void *psession) {
	YangIpcMessageServerSession *session =
			(YangIpcMessageServerSession*) psession;
	if (psession == NULL)
		return;

	return session->message.stop(&session->message.session);
}



int32_t yang_ipcmsg_publish(void *psession, char *topic, char *msg,
		int32_t msgLen) {
	YangIpcMessageServerSession *session =
			(YangIpcMessageServerSession*) psession;
	if (psession == NULL)
		return ERROR_MQTT_SERVER;

	return session->message.publish(&session->message.session, topic, msg,
			msgLen);
}

int32_t yang_ipcmsg_addCandidate(void* psession,int32_t uid,char* candidateStr){
	YangIpcMessageServerSession *session =
			(YangIpcMessageServerSession*) psession;
	if (psession == NULL)
		return ERROR_MQTT_SERVER;

	YangIpcRequest request ;
	yang_memset(&request,0,sizeof(YangIpcRequest));
	request.connectType = YangM_ConnectType_P2p;
	request.requestType = YangIpcRequestCandidate;
	request.uid = uid;
	request.data = candidateStr;

	YangConnCid* connCid=yang_ipcmessage_getConnCidByUid(session,uid);

	if(connCid)
		yang_memcpy(request.cid,connCid->cid,Yang_Cid_Length);
	

	return yang_ipcmessage_sendmsg(&session->message.session, request.cid[0]==0?(char*)Yang_Topic_Client:request.cid,&request);

}

int32_t yang_checkLive(void *psession){
	YangIpcMessageServerSession *session =(YangIpcMessageServerSession*) psession;
	int32_t ts=0;
	YangConnCid *connCid=NULL;
	if (psession == NULL)
		return ERROR_MQTT_SERVER;

	for(int32_t i=0;i<session->connCids.vec.vsize;i++){
		connCid=session->connCids.vec.payload[i];
		ts=((yang_get_system_time()-connCid->ts))/(1000*1000);

		if(ts>Yang_LiveTime_Second){
			yang_ipcmessage_removeConnCidByUid(session,connCid->uid);

			if (session&&session->handle.close) {
				session->handle.close(connCid->uid, session->handle.user);
			}

		}
	}

	return Yang_Ok;
}



int32_t yang_create_ipcmessageServer(YangIpcMessageServer *message,
		YangIpcHandle *handle, YangIpcDataMsg *dataMsg,YangAVInfo* avinfo) {
	YangIpcMessageServerSession *session;
	YangIpcMessageCallback callback;
	yang_memset(&callback, 0, sizeof(YangIpcMessageCallback));
	if (message == NULL || handle == NULL)
		return ERROR_MQTT_SERVER;
	
	session = (YangIpcMessageServerSession*) yang_calloc(sizeof(YangIpcMessageServerSession), 1);
	message->session = session;

	yang_setLogLevel(5);
	yang_setLogFile(1);
	
	session->avinfo=avinfo;
	session->handle.user = handle->user;
	session->handle.connect = handle->connect;
	session->handle.addCandidate = handle->addCandidate;
	session->handle.setLocalDescription = handle->setLocalDescription;
	session->handle.close = handle->close;
	
	yang_create_YangConnCidVector2(&session->connCids);

	yang_mutex_init(&session->msgLock);

	if (dataMsg) {
		session->dataMsg.user = dataMsg->user;
		session->dataMsg.receiveMsg = dataMsg->receiveMsg;
	}

	callback.user = session;
	callback.handleMessage = yang_ipcmessage_handleMessage;

	
	session->avinfo->mqtt->enableMqttTls=yangfalse;
	yang_create_ipcmessage(&session->message, &callback, session->avinfo,yangfalse);

	message->start = yang_ipcmsg_start;
	message->stop = yang_ipcmsg_stop;
	message->publish = yang_ipcmsg_publish;
	message->addCandidate = yang_ipcmsg_addCandidate;
	message->checkLive = yang_checkLive;

	return Yang_Ok;
}

void yang_destroy_ipcmessageServer(YangIpcMessageServer *message) {
	if (message == NULL || message->session == NULL)
		return;
	YangIpcMessageServerSession *session =
			(YangIpcMessageServerSession*) message->session;
	yang_destroy_ipcmessage(&session->message);

	for (int32_t i = 0; i < session->connCids.vec.vsize; i++) {
		yang_free(session->connCids.vec.payload[i]);
	}
	session->connCids.clear(&session->connCids.vec);
	yang_thread_mutex_destroy(&session->msgLock);
	yang_free(message->session);
}
