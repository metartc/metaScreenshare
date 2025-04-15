//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YangCAudioEncoder2__H
#define YangCAudioEncoder2__H

#include <yangutil/yangavinfotype.h>

typedef struct{
	void* user;
	void (*onAudioData)(YangFrame* pframe,void* user);
	void (*onVideoData)(YangFrame* pframe,void* user);
}YangCEncoderCallback;

typedef struct{
	void* session;
	int32_t (*encoder)(void* session,YangFrame* pframe,YangCEncoderCallback* callback);
	void (*setUid)(void* session,int32_t uid);
	int32_t (*getUid)(void* session);
	void (*setAudioCodec)(void* session,YangAudioCodec codec);
	YangAudioCodec (*getAudioCodec)(void* session);
}YangCAudioEncoder;

#ifdef __cplusplus
extern "C"{
#endif

int32_t yang_create_audioEncoder(YangCAudioEncoder* encoder,YangAudioCodec codec,YangAudioInfo* audioInfo);
void yang_destroy_audioEncoder(YangCAudioEncoder* encoder);



#ifdef __cplusplus
}
#endif

#endif
