//
// Copyright (c) 2019-2024 yanggaofeng
//

#ifndef YANGUTIL_BUFFER_YANGBUFFER2_H_
#define YANGUTIL_BUFFER_YANGBUFFER2_H_
#include <yangutil/sys/YangThread.h>
#include <yangutil/yangtype.h>
#include <yangutil/yangavtype.h>


typedef struct{
	int32_t  mediaType;
	int32_t  uid;
	uint32_t  putIndex;
	uint32_t  getIndex;
	uint32_t  cacheNum;
	uint32_t  unitSize;
	uint32_t  size;
	uint32_t  nextIndex;
	yang_thread_mutex_t mlock;
	YangFrame** frames;
	//uint8_t* bufferManager;
}YangMediaBufferSession2;

typedef struct{
	YangMediaBufferSession2 session;
	void (*resetIndex)(YangMediaBufferSession2* session);
	void (*initFrames)(YangMediaBufferSession2* session,int32_t pnum,int32_t unitsize);

	void (*putFrame)(YangMediaBufferSession2* session,YangFrame* pframe);
	void (*getFrame)(YangMediaBufferSession2* session,YangFrame* pframe);
	YangFrame* (*getCurFrameRef)(YangMediaBufferSession2* session);
	int64_t (*getNextFrameTimestamp)(YangMediaBufferSession2* session);
	uint8_t* (*getFrameRef)(YangMediaBufferSession2* session,YangFrame* pframe);


}YangMediaBuffer2;

void yang_create_mediaBuffer2(YangMediaBuffer2* buf);
void yang_destroy_mediaBuffer2(YangMediaBuffer2* buf);

#endif /* YANGUTIL_BUFFER_YANGBUFFER_H_ */
