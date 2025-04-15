
//
// Copyright (c) 2019-2024 yanggaofeng
//
#ifndef INCLUDE_YANGUTIL_YANGAVTYPE_H_
#define INCLUDE_YANGUTIL_YANGAVTYPE_H_



#include <yangutil/yangavinfo.h>
YangSample* yang_sample_copy(YangSample* src);


#ifdef __cplusplus
extern "C"{
#include <yangutil/yangframebuffer.h>
}
#else
#include <yangutil/yangframebuffer.h>
#endif

#ifdef __cplusplus
class YangBufferManager{
public:
	YangBufferManager();
	YangBufferManager(int32_t  num,int32_t  bufsize);
	virtual ~YangBufferManager();
	void init(int32_t  num,int32_t  bufsize);
	uint8_t* getBuffer();
private:
	uint8_t* m_cache;
	int32_t  m_curindex;
	int32_t  m_unitsize;
	int32_t  m_size;
};
#endif
#endif /* INCLUDE_YANGUTIL_YANGAVTYPE_H_ */
