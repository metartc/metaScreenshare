//
// Copyright (c) 2019-2024 yanggaofeng
//


#ifndef SRC_YANGAVUTIL_YANGSPSPPS_H_
#define SRC_YANGAVUTIL_YANGSPSPPS_H_
#include <yangutil/yangtype.h>

#ifdef __cplusplus
extern "C"{
#endif

int32_t yang_h264_decode_spspps(uint8_t * buf,uint32_t  nLen,int32_t  *p_width,int32_t  *p_height,int32_t  *p_fps);
int32_t yang_h265_decode_sps(uint8_t* nalData,uint32_t nLen,uint32_t* width,uint32_t* height);
#ifdef __cplusplus
}
#endif

#endif /* SRC_YANGAVUTIL_YANGSPSPPS_H_ */
