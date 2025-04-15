//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGCAPTURE_SRC_YangScreenWin_H_
#define YANGCAPTURE_SRC_YangScreenWin_H_
#include <yangcapture/YangScreenCapture.h>
#include "yangutil/buffer/YangVideoBuffer.h"
#include <yangutil/yangavinfotype.h>

class YangScreenWin:public YangThread {
public:
	YangScreenWin(YangAVInfo* avinfo);
	virtual ~YangScreenWin();

	int32_t init();
	void setInterval(int32_t pinterval);
	void setDrawmouse(yangbool isDraw);
	yangbool getisDrawmouse();

	void stop();
	
	
	uint8_t* getFrame();
	YangVideoBuffer* getOutVideoBuffer();
protected:
	void run() ;
	void startLoop();
	void stopLoop();

	void pushVideo();
	void initScreen();
private:	
	yangbool m_isDrawmouse;
	yangbool m_loop;
	yangbool m_isStart;
	int32_t m_pixelBits;
	int32_t m_interval;

	YangAVInfo* m_avinfo;
	YangVideoBuffer* m_out_videoBuffer;

    YangFrame m_screenFrame,m_videoFrame;
	
#ifdef _MSC_VER
	 char m_deskName[260];
#endif
};

#endif /* YANGCAPTURE_SRC_YANGSCREENCAPTUREIMPL_H_ */
