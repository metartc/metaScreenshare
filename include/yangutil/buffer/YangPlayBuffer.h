//
// Copyright (c) 2019-2024 yanggaofeng
//

#ifndef INCLUDE_YANGUTIL_BUFFER_YANGPLAYBUFFER_H_
#define INCLUDE_YANGUTIL_BUFFER_YANGPLAYBUFFER_H_
#include <yangutil/sys/YangThread.h>
class YangPlayBuffer {
public:
	YangPlayBuffer(uint32_t bufferLen);
	virtual ~YangPlayBuffer();
        int32_t putBuffer(uint8_t* buffer,uint32_t nb);
        int32_t gutBuffer(uint8_t* buffer,uint32_t nb);
	uint32_t size();
	//char* getBufferRef(uint32_t nb);
private:
	yang_thread_mutex_t m_lock;
	uint32_t m_maxSize;
	uint32_t m_size;
	uint32_t m_pos;
	uint8_t* m_buffer;
};

#endif /* INCLUDE_YANGUTIL_BUFFER_YANGPLAYBUFFER_H_ */
