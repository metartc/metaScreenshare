//
// Copyright (c) 2019-2023 yanggaofeng
//

#ifndef SRC_YANGCODEC_YANGAUDIODECODEROPUS2_H_
#define SRC_YANGCODEC_YANGAUDIODECODEROPUS2_H_
#include <yangutil/yangtype.h>
#include <yangutil/yangavctype.h>


typedef struct{
	void* user;
	void (*onAudioData)(YangFrame* pframe,void* user);
	void (*onVideoData)(YangFrame* pframe,void* user);
}YangCDecoderCallback;

typedef struct{
	void* session;
	int32_t (*decode)(void* session,YangFrame* pframe,YangCDecoderCallback* callback);
	void (*setUid)(void* session,int32_t uid);
	int32_t (*getUid)(void* session);
	void (*setAudioCodec)(void* session,YangAudioCodec codec);
	YangAudioCodec (*getAudioCodec)(void* session);
}YangCAudioDecoder;

#ifdef __cplusplus
extern "C"{
#endif


int32_t yang_create_audioDecoder(YangCAudioDecoder* decoder,YangAudioCodec codec,int32_t sample,int32_t channel,yangbool enableFec);
void yang_destroy_audioDecoder(YangCAudioDecoder* decoder);


#ifdef __cplusplus
}
#endif

#endif /* SRC_YANGCODEC_YANGAUDIODECODEROPUS2_H_ */
