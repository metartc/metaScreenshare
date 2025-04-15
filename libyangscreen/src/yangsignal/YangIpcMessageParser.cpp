//
// Copyright (c) 2019-2024 yanggaofeng
//

#include "YangIpcMessageParser.h"
#include <yangutil/sys/YangCString.h>
#include <yangutil/sys/YangLog.h>
#include <yangjson/YangJson.h>
#include <yangmqtt/YangMqtt.h>

int32_t yang_ipcmsg_parse(char* msg,YangIpcRequest* request){
	if(msg==NULL || request==NULL) return ERROR_STRING;
	int32_t err = Yang_Ok;
	YangJsonReader reader;
	if(yang_create_jsonReader(&reader,msg)!=Yang_Ok){
		return yang_error_wrap(ERROR_STRING,"read json error!");
	}

	YangJson* uid = reader.getObjectItemCaseSensitive(reader.session, "uid");
	YangJson* cid = reader.getObjectItemCaseSensitive(reader.session, "cid");
	YangJson* isHttps = reader.getObjectItemCaseSensitive(reader.session, "isHttps");
	YangJson* connectType = reader.getObjectItemCaseSensitive(reader.session, "connectType");
	YangJson* requestType = reader.getObjectItemCaseSensitive(reader.session, "requestType");
	YangJson* url = reader.getObjectItemCaseSensitive(reader.session, "url");
	YangJson* data = reader.getObjectItemCaseSensitive(reader.session, "data");

	if (reader.isNumber(uid))
	{
		request->uid=uid->valueint;
	}

	if (reader.isString(cid) && (cid->valuestring != NULL))
	{
		yang_memcpy(request->cid,cid->valuestring,yang_min(Yang_Cid_Length-1,yang_strlen(cid->valuestring)));
	}

	request->isHttps=reader.isTrue(isHttps)?yangtrue:yangfalse;

	if (reader.isNumber(connectType))
	{
		request->connectType=(YangIpcConnectType)connectType->valueint;
	}

	if (reader.isNumber(requestType))
	{		
		request->requestType=(YangIpcRequestType)requestType->valueint;
	}

	if (reader.isString(url) && (url->valuestring != NULL))
	{
		request->url=(char*)yang_calloc(yang_strlen(url->valuestring)+1,1);
		yang_memcpy(request->url,url->valuestring,yang_strlen(url->valuestring));
	}

	if (reader.isString(data) && (data->valuestring != NULL))
	{
		request->data=(char*)yang_calloc(yang_strlen(data->valuestring)+1,1);
		yang_memcpy(request->data,data->valuestring,yang_strlen(data->valuestring));

	}

	yang_destroy_jsonReader(&reader);

	return err;
}


int32_t yang_ipcmsg_send(YangMqtt* mqtt,char* topic,YangIpcRequest* request){
	YangJsonWriter writer;
	yang_create_jsonWriter(&writer);

	writer.addNumberToObject(writer.session,"uid",request->uid);
	writer.addStringToObject(writer.session,"cid",request->cid);
	writer.addNumberToObject(writer.session,"requestType",request->requestType);
	writer.addNumberToObject(writer.session,"connectType",request->connectType);
	writer.addStringToObject(writer.session,"url",request->url);
	writer.addNumberToObject(writer.session,"isHttps",request->isHttps);
	writer.addStringToObject(writer.session,"data",request->data);
	char* jsonStr=writer.printUnformatted(writer.session);

	mqtt->publish(mqtt->session,topic,jsonStr,yang_strlen(jsonStr));
	yang_destroy_jsonWriter(&writer);
	return Yang_Ok;
}





