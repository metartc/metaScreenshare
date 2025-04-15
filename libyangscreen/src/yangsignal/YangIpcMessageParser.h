//
// Copyright (c) 2019-2024 yanggaofeng
//

#ifndef SRC_YANGSIGNAL_YANGIPCMESSAGEPARSER_H_
#define SRC_YANGSIGNAL_YANGIPCMESSAGEPARSER_H_
#include <yangsignal/YangIpcMessageDef.h>
#include <yangmqtt/YangMqtt.h>
int32_t yang_ipcmsg_parse(char* msg,YangIpcRequest* request);
int32_t yang_ipcmsg_send(YangMqtt* mqtt,char* topic,YangIpcRequest* request);
#endif /* SRC_YANGSIGNAL_YANGIPCMESSAGEPARSER_H_ */
