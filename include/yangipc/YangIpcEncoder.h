//
// Copyright (c) 2019-2023 yanggaofeng
//

#ifndef INCLUDE_YANGIPC_YANGIPCENCODER_H_
#define INCLUDE_YANGIPC_YANGIPCENCODER_H_

#include <yangutil/yangavinfo.h>
#include <yangutil/sys/YangThread.h>
#include <yangutil/buffer2/YangFrameBuffer.h>



typedef struct{
	yangbool isStart;
	yangbool isLoop;
	yang_thread_t threadId;
	YangFrameBuffer *out_audioBuffer;
}YangEncoderAudioSession;

typedef struct{
	void* session;
	int32_t (*init)(void* session,YangCodecEnable *codecEnable,YangCodecCallback *callback);
	int32_t (*start)(void* session);
	void (*stop)(void* session);
	void (*sendMsgToEncoder)(void* session,YangRequestType request);

}YangEncoderVideo;

typedef struct{
	YangEncoderAudioSession session;
	int32_t (*init)(YangEncoderAudioSession* session);
	int32_t (*start)(YangEncoderAudioSession* session);
	void (*stop)(YangEncoderAudioSession* session);

}YangEncoderAudio;



int32_t yang_create_videoEncoder(YangEncoderVideo* encoder,YangAVInfo* avinfo);
void yang_destroy_videoEncoder(YangEncoderVideo* encoder);

int32_t yang_create_audioEncoder(YangEncoderAudio* encoder,YangAVInfo* avinfo);
void yang_destroy_audioEncoder(YangEncoderAudio* encoder);



#endif /* INCLUDE_YANGIPC_YANGIPCENCODER_H_ */
