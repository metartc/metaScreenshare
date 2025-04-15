//
// Copyright (c) 2019-2024 yanggaofeng
//
#ifndef INCLUDE_YANGAVUTIL_VIDEO_YANGCNALU_H_
#define INCLUDE_YANGAVUTIL_VIDEO_YANGCNALU_H_

#include <yangutil/yangtype.h>
#include <yangutil/yangavtype.h>
#include <stdint.h>

#define Yang_Nalu_SpsppsBufferLen 256

//remove sei nalu
int32_t yang_nalu_formatH264IDR(YangFrame* frame);
int32_t yang_nalu_parseH264IDR();
int32_t yang_nalu_getH264KeyframePos( uint8_t* data,int32_t nb);

int32_t  yang_nalu_getH264IDR(YangFrame* videoFrame);
yangbool yang_nalu_hasH264PFrame(uint8_t* p);

//remove sei nalu
int32_t yang_nalu_formatH265IDR(YangFrame* frame);
int32_t yang_nalu_getH265KeyframePos( uint8_t* data,int32_t nb);

int32_t  yang_nalu_getPos(uint8_t* data,int32_t plen);

#endif /* INCLUDE_YANGAVUTIL_VIDEO_YANGCNALU_H_ */
