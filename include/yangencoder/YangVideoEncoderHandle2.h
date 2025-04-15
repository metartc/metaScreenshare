//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef __YangVideoEncoderHandle22__
#define __YangVideoEncoderHandle22__

#include <yangencoder/YangVideoEncoder.h>
#include <yangutil/buffer2/YangFrameBuffer.h>
#include <yangutil/buffer/YangVideoBuffer.h>
#include <yangutil/sys/YangThread2.h>

#include <vector>
using namespace std;

class YangVideoEncoderHandle2:public YangThread,public YangEncoderCallback
{
public:
	YangVideoEncoderHandle2(YangAVInfo* avinfo);
	~YangVideoEncoderHandle2(void);

private:
	int32_t m_isInit;

public:
	int32_t m_isStart;
	int32_t m_uid;
	yang_encoder_enable m_enableEncode;
	void init();
	void stop();
	void setOutVideoBuffer(YangFrameBuffer * pvl);
	void setInVideoBuffer(YangVideoBuffer *pvl);


	void setVideoMetaData(YangVideoMeta *pvmd);
	void sendMsgToEncoder(YangRequestType request);
	void onVideoData(YangFrame* pframe);
	void onAudioData(YangFrame* pframe);
protected:
	void run();
	void stopLoop();
	void startLoop();
	void saveFile(char *fileName, uint8_t *pBuffer, long BufferLen);

private:
	YangAVInfo* m_avinfo;
	YangVideoBuffer *m_in_videoBuffer;
	YangFrameBuffer *m_out_videoBuffer;
	YangVideoMeta *m_vmd;

	int32_t m_isConvert;
	uint8_t* m_keyframeBuffer;
	int64_t m_curtimestamp,m_lastTimestamp;
	int32_t m_sendMsgToEncoder;
	bool m_keyframeRequest;
	YangFrame m_keyframe;
	YangRtcEncoderMessage m_request;

};
#endif
