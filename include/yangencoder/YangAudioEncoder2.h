//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef ___YangAudioEncoder2__
#define ___YangAudioEncoder2__

#include <yangutil/yangavinfotype.h>


typedef struct{
	YangAudioInfo* audioInfo;
	void* user;
	void (*onAudioData)(YangFrame* pframe);
}YangAudioEncoderSession;

typedef struct{
	void* session;
	int32_t (*encoder)(void* session,YangFrame* pframe);

}YangAudioEncoder2;

int32_t yang_create_audioEncoderOpus(YangAudioEncoder2* encoder,YangAudioInfo* audioInfo);
void yang_destroy_audioEncoderOpus(YangAudioEncoder2* encoder);

#endif
