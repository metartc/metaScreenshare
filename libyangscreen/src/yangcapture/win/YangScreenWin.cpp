//
// Copyright (c) 2019-2023 yanggaofeng
//
#include <yangcapture/win/YangScreenWin.h>

#ifdef _WIN32
#include <yangutil/sys/YangTime.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/yangavinfotype.h>
#include <yangavutil/video/YangYuvConvert.h>
#include <yangscreen/YangScreen.h>
#include <yangipc/YangIpcConfig.h>
#if Yang_Enable_ScreenCaptureDXGI
#include <yangcapture/win/YangDXGIManager.h>
YangDXGIManager *g_screenCapture=NULL;
#else
#include <yangcapture/win/YangGDIManager.h>

YangGDIManager *g_screenCapture=NULL;
#endif



void YangScreenWin::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}
void YangScreenWin::stop() {
	stopLoop();

}

int32_t YangScreenWin::init() {

	if (m_out_videoBuffer == NULL)
		m_out_videoBuffer = new YangVideoBuffer(
				m_avinfo->video.bitDepth == 8 ? 1 : 2);

	return Yang_Ok;
}

YangScreenWin::YangScreenWin(YangAVInfo *avinfo) {
	m_avinfo = avinfo;
	m_out_videoBuffer = nullptr;

	m_isStart = yangfalse;
	m_loop = yangfalse;

	m_interval=10;
	m_isDrawmouse=false;
	m_pixelBits = 32;

	memset(&m_videoFrame, 0, sizeof(YangFrame));
}

YangScreenWin::~YangScreenWin() {
	if(m_isStart){
		stop();
		yang_stop_thread(this);
	}

	yang_delete(m_out_videoBuffer);
	yang_delete(g_screenCapture);
}



YangVideoBuffer* YangScreenWin::getOutVideoBuffer() {
	return m_out_videoBuffer;
}


void YangScreenWin::setDrawmouse(yangbool isDraw) {
	m_isDrawmouse=isDraw;
}

yangbool YangScreenWin::getisDrawmouse() {
	return m_isDrawmouse;
}

void YangScreenWin::pushVideo(){

	if (m_out_videoBuffer&&m_screenFrame.payload){
		m_out_videoBuffer->putVideo(&m_screenFrame);
	}

}

uint8_t* YangScreenWin::getFrame() {
	if (m_out_videoBuffer == NULL)
		return NULL;
	return m_out_videoBuffer->getVideoRef(&m_videoFrame);
}

void YangScreenWin::initScreen(){
	
	if(g_screenCapture==NULL) {

#if Yang_Enable_ScreenCaptureDXGI
		CoInitialize(NULL);
		g_screenCapture=new YangDXGIManager();

#else
		g_screenCapture=new YangGDIManager(m_pixelBits);
#endif
	}
}

void YangScreenWin::stopLoop() {
	m_loop = yangfalse;
}


void YangScreenWin::startLoop() {
#ifdef _MSC_VER
	init();
#if 0
	m_loop = yangtrue;
	while (m_loop) {
		if (isLogin()) break;
		Sleep(1000);
	}
#endif	

	YangScreen::toCurrent(false);
	initScreen();

	g_screenCapture->init();

	yang_trace("\nscreen capture init success");

#if Yang_Enable_ScreenCaptureDXGI
	//CSMonitor1 CSDesktop
	g_screenCapture->SetCaptureSource(CSDesktop);
#endif
	RECT rcDim;
	g_screenCapture->GetOutputRect(rcDim);

	int dwWidth = rcDim.right - rcDim.left;
	int dwHeight = rcDim.bottom - rcDim.top;
	m_avinfo->video.width=dwWidth;
	m_avinfo->video.outWidth=dwWidth;
	m_avinfo->video.height=dwHeight;
	m_avinfo->video.outHeight=dwHeight;

	if (m_out_videoBuffer == NULL)
		m_out_videoBuffer = new YangVideoBuffer(
				m_avinfo->video.bitDepth == 8 ? 1 : 2);

	m_out_videoBuffer->init(dwWidth, dwHeight, m_avinfo->video.videoEncoderFormat);

	yang_trace("\ndwWidth=%d dwHeight=%d,bits=%d\n", dwWidth, dwHeight, m_pixelBits);

	DWORD dwBufSize = dwWidth * dwHeight * 4;
	int yuvLen=dwWidth * dwHeight * 3 / 2;
	YangYuvConvert yuv;
	uint8_t *pBuf = new uint8_t[dwWidth * dwHeight * 5];
	uint8_t *dst = new uint8_t[yuvLen];

	memset(&m_screenFrame,0,sizeof(YangFrame));

	m_screenFrame.payload = dst;
	m_screenFrame.nb = dwWidth * dwHeight * 3 / 2;
	if(m_avinfo->video.videoEncoderFormat==YangArgb) {
		m_screenFrame.nb = dwWidth * dwHeight * 4;
		m_screenFrame.payload=pBuf;
	}
	HRESULT hr = 0;
	m_loop = yangtrue;
	int32_t interval = m_interval;

	while (m_loop) {

		if (!yang_ipc_encoder_enable()){
			yang_usleep(interval *1000);
			continue;
		}

		if (m_out_videoBuffer->m_loginState) {
			interval = 100;
			if (!YangScreen::isWinlogin()) {
				m_out_videoBuffer->m_loginState = yangfalse;
				interval = m_interval;
			}else {
				continue;
			}
		}
		else {
			interval = m_interval;
		}
		g_screenCapture->m_isDrawmouse=m_isDrawmouse;
		hr = g_screenCapture->GetOutputBits(pBuf, rcDim);
		if (FAILED(hr)) {
			if(hr==DXGI_ERROR_WAIT_TIMEOUT){
				//yang_error("GetOutputBits failed because DXGI_ERROR_WAIT_TIMEOUT\n");
			}else if(hr==DXGI_ERROR_ACCESS_LOST){
				yang_error("GetOutputBits failed because DXGI_ERROR_ACCESS_LOST\n");
#if !Yang_Enable_ScreenCaptureDXGI
				g_screenCapture->init2();
#endif
			}else
				yang_error("GetOutputBits failed with hr=0x%08x\n", hr);

		} else {
			if(m_avinfo->video.videoEncoderFormat==YangI420) {
#if Yang_Enable_ScreenCaptureDXGI
				yuv.bgratoI420(pBuf, dst, dwWidth, dwHeight);
#else
				if (m_pixelBits == 32) {

					yuv.bgratoI420(pBuf, dst, dwWidth, dwHeight);
				}
				else {
					yuv.rgb24toI420(pBuf, dst, dwWidth, dwHeight);
				}
#endif
			}else if(m_avinfo->video.videoEncoderFormat==YangNv12) {
#if Yang_Enable_ScreenCaptureDXGI
				yuv.bgratonv12(pBuf, dst, dwWidth, dwHeight);
#else
				if (m_pixelBits == 32) {

					yuv.bgratonv12(pBuf, dst, dwWidth, dwHeight);
				}
				else {
					yuv.rgb24toI420(pBuf, dst, dwWidth, dwHeight);
				}
#endif
			}

			pushVideo();

		}
		yang_usleep(interval *1000);

	}
	m_screenFrame.payload=NULL;
	yang_deleteA(pBuf);
	yang_deleteA(dst);
	yang_delete(g_screenCapture);
#if 0
	if (!SetThreadDesktop(currentDesk))
	{

		yang_error("SetThreadDesktop currentDesk error(%lu)", GetLastError());
		return;
	}
#endif


#if Yang_Enable_ScreenCaptureDXGI
	CoUninitialize();
#endif


#endif
}

void YangScreenWin::setInterval(int32_t pinterval) {
	m_interval=pinterval;
}

#endif
