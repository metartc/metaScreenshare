//
// Copyright (c) 2019-2024 yanggaofeng
//
#ifndef __YangFrameBuffer2__
#define __YangFrameBuffer2__
#include <yangutil/yangtype.h>
#include <yangutil/yangavinfo.h>
#include <string.h>

#define yang_reindex2(p)  if(p!=NULL) p->resetIndex(p->session);

typedef struct{
	void* session;
	int32_t  (*getBuffer)(void* session,YangFrame* pframe);
	uint8_t * (*getBufferRef)(void* session,YangFrame* frame);
	void (*putBuffer)(void* session,YangFrame* pframe);
	uint32_t  (*size)(void* session);
	void (*resetIndex)(void* session);

	void (*setUid)(void* session,int32_t  uid);
	int32_t  (*getUid)(void* session);

	void (*setMediaType)(void* session,int32_t  uid);
	int32_t  (*getMediaType)(void* session);

}YangFrameBuffer;
#ifdef __cplusplus
extern "C"{
#endif
void yang_create_frameBuffer(YangFrameBuffer* buffer,int32_t  pcachenum,int32_t  bufLen);
void yang_destroy_frameBuffer(YangFrameBuffer* buffer);
#ifdef __cplusplus
}
#endif
#endif
