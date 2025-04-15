//
// Copyright (c) 2019-2023 yanggaofeng
//

#ifndef INCLUDE_YANGIPC2_YANGIPCAPP_H_
#define INCLUDE_YANGIPC2_YANGIPCAPP_H_
#include <yangapp/yangipc/YangIpc.h>
#include <yangapp/yangipc/YangIpcConfig.h>

#include <yangutil/sys/YangCTimer.h>
typedef struct{
	yangbool isInit;
	YangIpc* ipc;
	YangCTimer* timer_5s;
	YangAVInfo *avinfo;
	YangEncoderAudio* audioEncoder;
	YangEncoderVideo *videoEncoder;

}YangIpcAppSession;

typedef struct {
	YangIpcAppSession session;
	int32_t (*init)(YangIpcAppSession* session,YangIpcDataMsg* channelmsg);
	uint32_t (*getConnectionCount)(YangIpcAppSession* session);
	int32_t (*start)(YangIpcAppSession* session);
	void (*stop)(YangIpcAppSession* session);
}YangIpcApp;

#ifdef __cplusplus
extern "C"{
#endif
void yang_create_ipcApp(YangIpcApp* app);
void yang_destroy_ipcApp(YangIpcApp* app);
#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_YANGIPC2_YANGIPCAPP_H_ */
