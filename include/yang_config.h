//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANG_CONFIG_H_
#define INCLUDE_YANG_CONFIG_H_

#include "yang_config_os.h"

#define Yang_Enable_Datachannel   1
#define Yang_Enable_Mqtt    	  1
#define Yang_Enable_Json    	  1
#define Yang_Enable_Openh264      1
#define Yang_Enable_Mpp           0
#define Yang_Enable_RECURSIVE     0
#define Yang_Enable_Audio_Poll    0
#define Yang_Enable_USLEEP        1
#define Yang_Enable_Debug         1
#define Yang_Enable_Opus          0

#if Yang_OS_WIN
#define Yang_Enable_Openssl       1
#define Yang_Enable_GPU_Encoding  1
#define Yang_Enable_Phtread       0
#else
#define Yang_Enable_Openssl       1
#define Yang_Enable_GPU_Encoding  0
#define Yang_Enable_Phtread       1
#endif

#define Yang_Enable_Meeting       1
#define Yang_MultThread_Decode    0

#define Yang_Enable_H265_Encoding 1
#define Yang_Enable_MJpeg_Encoding 0
#define Yang_Enable_AV1_Encoding  0

#define Yang_Enable_RTC_Audio     1
#define Yang_Enable_RTC_Video     1

#define Yang_Enable_Tcp_Srs       1

#define Yang_Enable_WebrtcAec     1

#define Yang_Enable_Embbed        0
#define Yang_Enable_10bit         0

#if __BYTE_ORDER == __BIG_ENDIAN
#define Yang_Little_Endian 0
#else
#define Yang_Little_Endian 1
#endif

#define Yang_Enable_LinuxScreen   0
#define Yang_Enable_Dtls          1 //default:using dtls
#define Yang_Enable_Tls           1
#define Yang_Enable_TWCC          1


#define Yang_Enable_H264Decoder 1
#define Yang_Enable_H264Decoder_So 0

#if Yang_Enable_H264Decoder
#define Yang_Enable_Ffmpeg_Codec   0
#define Yang_Enable_FfmpegSo       0
#else
#define Yang_Enable_Ffmpeg_Codec 1
#define Yang_Enable_FfmpegSo 1
#endif

#define Yang_Enable_Timer_Phtread 0

#define Yang_Enable_Sysbyteswap 1
#define Yang_gatherTurn_WaitTime 2000 //ms

//option lib
#define Yang_Enable_PicUtilFfmpeg 0
#define Yang_Enable_Websockets 1 //using libwebsockets
#define Yang_Enable_Curl 0   //using libcurl


#define Yang_Enable_RtcpXr 0
#define Yang_Rtp_I_Withmeta 1
#define Yang_CTimer_Epoll_Timeout 200
#define Yang_Enable_Vr 0
#endif /* INCLUDE_YANG_CONFIG_H_ */
