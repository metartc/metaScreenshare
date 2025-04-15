//
// Copyright (c) 2019-2024 yanggaofeng
//

#ifndef INCLUDE_YANGAVUTIL_VIDEO_YANGCMETA_H_
#define INCLUDE_YANGAVUTIL_VIDEO_YANGCMETA_H_

#include <yangutil/yangavtype.h>

int32_t yang_meta_parseH264(YangH2645Meta* meta,uint8_t *payload,int32_t  bufLen);
int32_t yang_meta_parseH265(YangH2645Meta* meta,uint8_t *payload,int32_t  bufLen);

int32_t  yang_find_start_code(uint8_t *buf,int32_t  bufLen);

int32_t yang_meta_createH264( YangH2645Conf* conf, uint8_t* payload,int32_t nb);
int32_t yang_meta_createH265( YangH2645Conf* conf, uint8_t* payload,int32_t nb);

int32_t yang_meta_getH264Sps(uint8_t *payload, int32_t nb, uint8_t **sps, int32_t *pspsLen);
int32_t yang_meta_getH265Sps(uint8_t *payload, int32_t nb, uint8_t **sps, int32_t *pspsLen);

int32_t yang_meta_getH264FlvSps(uint8_t *payload, int32_t nb, uint8_t **sps, int32_t *pspsLen);
int32_t yang_meta_getH265FlvSps(uint8_t *payload, int32_t nb, uint8_t **sps, int32_t *pspsLen);

int32_t yang_meta_parseH264Header(uint8_t *payload,int32_t nb, uint8_t *src, int32_t *hLen);
int32_t yang_meta_parseH265Header(uint8_t *payload,int32_t nb, uint8_t *src, int32_t *hLen);

int32_t yang_meta_decodeFlvH264(uint8_t *configBuf,int32_t  p_configLen, YangSample* sps, YangSample* pps);
int32_t yang_meta_decodeFlvH265(uint8_t *configBuf,int32_t  p_configLen, YangSample* vps, YangSample* sps, YangSample* pps);

void yang_meta_getH264Flv( YangH2645Conf *p_264, uint8_t *configBuf,int32_t  *p_configLen);
void yang_meta_getH264FlvBySample( YangSample* sps,  YangSample* pps,uint8_t *configBuf,int32_t  *p_configLen);

void yang_meta_getH265Flv( YangH2645Conf *p_264, uint8_t *configBuf,int32_t  *p_configLen);
void yang_meta_getH265FlvBySample( YangSample* vps, YangSample* sps,  YangSample* pps, uint8_t *configBuf,int32_t  *p_configLen);


#endif /* INCLUDE_YANGAVUTIL_VIDEO_YANGCMETA_H_ */
