/*
 * YangRtcPacer.h
 *
 *  Created on: 2025年2月27日
 *      Author: yang
 */

#ifndef SRC_YANGRTCPACER_H_
#define SRC_YANGRTCPACER_H_
#include "YangPeerConnection8.h"
class YangRtcPacer {
public:
	YangRtcPacer(){};
	virtual ~YangRtcPacer(){};
public:
	virtual int32_t reset()=0;
	virtual int32_t process()=0;

	virtual int32_t resetAudioTime()=0;
	virtual int32_t resetVideoTime()=0;
	virtual int32_t writeAudioFrame(uint8_t* data,int32_t nb)=0;
	//frametype:YANG_Frametype_I YANG_Frametype_P
	//not pace mode
	virtual int32_t writeVideoFrame(uint8_t* data,int32_t nb,uint64_t pts,int32_t frametype)=0;
	virtual int32_t bufferAudioFrame(uint8_t* data,int32_t nb)=0;
	//frametype:YANG_Frametype_I YANG_Frametype_P
	//pace mode
	virtual int32_t bufferVideoFrame(uint8_t* data,int32_t nb,uint64_t pts,int32_t keyframetype)=0;
	virtual int32_t attachConnection(YangPeerConnection8* conn)=0;
};

YangRtcPacer* yang_create_rtcPacer(YangPeerInfo* peerInfo);

#endif /* SRC_YANGRTCPACER_H_ */
