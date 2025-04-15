//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef __YangAudioEncoderrHandle2__
#define __YangAudioEncoderrHandle2__

#include <yangutil/buffer/YangAudioEncoderBuffer.h>
#include <yangutil/buffer2/YangFrameBuffer.h>
#include <yangutil/sys/YangThread.h>
#include <yangencoder/YangAudioEncoder.h>
#include <vector>


using namespace std;
class YangAudioEncoderHandle2:public YangThread,public YangEncoderCallback
{
public:
	YangAudioEncoderHandle2(YangAudioInfo *pcontext);
	~YangAudioEncoderHandle2(void);

private:
	int32_t m_isInit;

public:
	void init();
	void stop();
	void setInAudioBuffer(YangAudioBuffer *pbuf);
	void setOutAudioBuffer(YangFrameBuffer *pbuf);

	void onVideoData(YangFrame* pframe);
	void onAudioData(YangFrame* pframe);
	int32_t m_isStart;
	int32_t m_uid;
	yang_encoder_enable m_enableEncode;


protected:
	void run();
	void stopLoop();
	void startLoop();

void saveFile(char *fileName, uint8_t *pBuffer, long BufferLen);

private:

	YangAudioEncoder *m_enc;
	int32_t m_isConvert;
	YangAudioBuffer *m_in_audioBuffer;
	YangFrameBuffer *m_out_audioBuffer;
	YangAudioInfo *m_context;
	void Encoder(int32_t isIframe,uint8_t*src,int32_t  p_buflen,uint8_t *dest,int32_t *p_destLen);

};
#endif
