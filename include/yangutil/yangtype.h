//
// Copyright (c) 2019-2024 yanggaofeng
//
#ifndef INCLUDE_YANGUTIL_YANGTYPE_H_
#define INCLUDE_YANGUTIL_YANGTYPE_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <yang_config.h>
#include <yangutil/yangunistd.h>
#include <yangutil/yangerrorcode.h>
#include <yangutil/yangmemory.h>

#define Yang_Server_Srs 0
#define Yang_Server_Zlm 1
#define Yang_Server_P2p 7
#define Yang_Server_Whip_Whep 9

#define Yang_Rtmp 0
#define Yang_Srt 1
#define Yang_Webrtc 2

typedef  uint8_t yangbool;
#define yangtrue 1
#define yangfalse 0

#define yang_delete(a) {if( (a)) {delete (a); (a) = NULL;}}
#define yang_deleteA(a) {if( (a)) {delete[] (a); (a) = NULL;}}
#define yang_free(a) {if( (a)) {free((a)); (a) = NULL;}}
#define yang_min(a, b) (((a) < (b))? (a) : (b))
#define yang_max(a, b) (((a) < (b))? (b) : (a))
#define yang_abs(a, b) ((a) > (b) ? ((a) - (b)) : ((b) - (a)))
#define	yang_get_bit(x, bit)	((x & (1 << bit)) >> bit)

#define YANG_UTIME_MILLISECONDS 1000
#define YANG_UTIME_SECONDS 		1000000

#define YANG_VIDEO_ENCODE_BUFFER_LEN 2048 * 1024
#define YANG_RTC_RECV_BUFFER_COUNT 1024
#define YANG_RTC_RECV_BUFFER_COUNT2 1023

#define YANG_GET_RECV_BUFFER_INDEX(x) 	 x&YANG_RTC_RECV_BUFFER_COUNT2

#define Yang_AV1_Name "AV1X"

#define YANGALIGN(x, a) (((x)+(a)-1)&~((a)-1))

#define YANG_INADDR_ANY 0x00000000

typedef enum{
	Yang_Socket_Protocol_Udp,
	Yang_Socket_Protocol_Tcp
}YangSocketProtocol;

typedef enum {
	Yang_IpFamilyType_IPV4,
	Yang_IpFamilyType_IPV6
} YangIpFamilyType;

typedef struct{
	int16_t mediaType;
	int16_t frametype;
	int32_t capacity;

	int32_t uid;
	int32_t nb;

	int64_t pts;
	int64_t dts;
	uint8_t* payload;
}YangFrame;


#endif /* INCLUDE_YANGUTIL_YANGTYPE_H_ */
