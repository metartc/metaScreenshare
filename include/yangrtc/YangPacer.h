//
// Copyright (c) 2019-2024 yanggaofeng
//


#ifndef INCLUDE_YANGPUSH_YANGPACER_H_
#define INCLUDE_YANGPUSH_YANGPACER_H_

#include <yangrtc/YangPeerConnection.h>

typedef struct{
	void* session;
	int32_t (*on_audio)(void* session,YangPushData* pushData);
	int32_t (*on_video)(void* session,YangPushData* pushData);
}YangPacerCallback;


typedef struct{
	void* session;
	int32_t (*reset)(void* session);
	int32_t (*process)(void* session);

	int32_t (*resetAudioTime)(void* session);
	int32_t (*resetVideoTime)(void* session);
	int32_t (*writeAudioFrame)(void* session,YangFrame* frame);
	int32_t (*writeVideoFrame)(void* session,YangFrame* frame);
	int32_t (*bufferAudioFrame)(void* session,YangFrame* frame);
	int32_t (*bufferVideoFrame)(void* session,YangFrame* frame);
	int32_t (*attachConnection) (void* session,YangPeerConnection* conn);

}YangPacer;

#ifdef __cplusplus
extern "C" {
#endif

int32_t yang_create_pacer(YangPacer* pacer,YangPeerInfo* peerInfo,yangbool hasAudio,yangbool hasVideo,YangPacerCallback* callback);
void yang_destroy_pacer(YangPacer* pacer);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_YANGPUSH_YANGPACER_H_ */
