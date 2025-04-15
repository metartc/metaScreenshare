//
// Copyright (c) 2019-2024 yanggaofeng
//
#ifndef __YangVideoDecoderBuffer__
#define __YangVideoDecoderBuffer__

#include <stdint.h>
#include <yangutil/buffer/YangMediaBuffer.h>
class YangVideoDecoderBuffer:public YangMediaBuffer
{
public:
	YangVideoDecoderBuffer(int32_t num);
	~YangVideoDecoderBuffer(void);

	void putEVideo(YangFrame* pframe);
	void getEVideo(YangFrame* pframe);
private:


};
#endif
