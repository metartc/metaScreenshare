//
// Copyright (c) 2019-2024 yanggaofeng
//

#ifndef YANGUTIL_YANGAVINFOTYPE_H_
#define YANGUTIL_YANGAVINFOTYPE_H_

#include <yangutil/yangtype.h>
#include <yangutil/yangcodec.h>
#include <yangutil/yangavtype.h>



#ifdef __cplusplus
#include <yangstream/YangStreamManager.h>
#include <yangstream/YangSynBufferManager.h>
class YangContext {
public:
	YangContext();
	virtual ~YangContext();
	void init(char *filename);
	void initMqttInfo(char *filename);
	void initFileInfo(char *filename);
	void init();

	virtual void initExt(void *filename);
	virtual void initExt();
public:

	YangAVInfo avinfo;
	YangSendRequestPli sendRtcPli;
	YangChannelDataRecvI channeldataRecv;
	YangChannelDataSendI channeldataSend;
#if Yang_OS_ANDROID
	void* nativeWindow;
#endif


	//YangSynBufferManager synMgr;
	//YangStreamManager* streams;
};
extern "C"{
void yang_init_avinfo(YangAVInfo* avinfo);
}
#else
void yang_init_avinfo(YangAVInfo* avinfo);
#endif

#endif /* YANGUTIL_YANGTYPE_H_ */
