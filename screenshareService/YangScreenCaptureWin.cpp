//
// Copyright (c) 2019-2023 yanggaofeng
//

#include "YangScreenCaptureWin.h"
#include <yangavutil/video/YangYuvConvert.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangIni.h>
#include "yangutil/sys/YangTime.h"
#include "yangutil/sys/YangLog.h"
#include "yangscreen.h"
#include <yangscreen/YangScreen.h>
#define GetSockError()	WSAGetLastError()
#define SetSockError(e)	WSASetLastError(e)

#if Yang_Enable_ScreenCaptureDXGI2
#include "yangcapture/win/YangDXGIManager.h"
YangDXGIManager* g_screenCapture3 = NULL;
#else
#include <yangcapture/win/YangDXGIManager.h>

#include "yangcapture/win/YangGDIManager.h"
YangGDIManager* g_screenCapture3 = NULL;
#endif

YangScreenCaptureWin::YangScreenCaptureWin(char* szPath)
{	m_rgb = NULL;
	m_yuv = NULL;
	m_bufLen = 0;

	m_pixelBits = 32;
	m_isLoop = yangfalse;
	m_isStart = yangfalse;
	m_state = 0;
	m_interval = 40;
	m_startTime = 0;
	m_isDrawmouse = false;

	char inifile[260] = { 0 };
	char logfile[260] = { 0 };
	sprintf(inifile, "%sdesktop_config.ini", szPath);
	sprintf(logfile, "%syang_log2.log", szPath);

	m_context = new YangContext();
	YangIni ini;
	ini.filename = NULL;
	yang_create_ini2(&ini, inifile);

	ini.initAudio(ini.filename, &m_context->avinfo.audio);
	ini.initVideo(ini.filename, &m_context->avinfo.video);
	ini.initSys(ini.filename, &m_context->avinfo.sys);
	ini.initEnc(ini.filename, &m_context->avinfo.enc);
	ini.initRtc(ini.filename, &m_context->avinfo.rtc);

	yang_destroy_ini(&ini);

	yang_setLogLevel(m_context->avinfo.sys.logLevel);
	yang_setCLogFile2(m_context->avinfo.sys.enableLogFile, logfile);
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, &wsaData);

	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	int32_t on = 1;
	setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&on, sizeof(on));

	int32_t timeout = 800;
	setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

	m_screenEvent = new YangScreenEvent(m_context);
}

YangScreenCaptureWin::~YangScreenCaptureWin()
{
	
	if (m_isStart) {
		stop();
		while (m_isStart) {
			Sleep(200);
		}
	}
	yang_delete(g_screenCapture3);
	yang_delete(m_context);
	yang_delete(m_screenEvent);
}

#define Yang_Http_Buffer 1024*10
DWORD WINAPI yang_run_loginserver_thread(LPVOID lpParam){

	YangScreenCaptureWin* server = (YangScreenCaptureWin*)lpParam;

	char* buf = (char*)malloc(Yang_Http_Buffer);

	memset(buf, 0, Yang_Http_Buffer);
	char* p = NULL;

	YangFrame m_frame;
	int32_t headerLen = 0;
	int32_t nBytes = 0;
	int32_t frameLen = 0;
	server->m_isLoop = yangtrue;
	yangbool recvDataStart = yangfalse;
	int32_t recvLen = 0;
	while (server->m_isLoop) {
		memset(buf, 0, Yang_Http_Buffer);
		nBytes = recv(server->m_socket, (char*)buf, Yang_Http_Buffer, 0);

		if (nBytes > 0) {

			server->m_screenEvent->screenEvent(buf, nBytes);
			continue;

		}
		else if (nBytes == -1) {
			int32_t sockerr = GetSockError();
			if (sockerr == EINTR)
				continue;
			if (sockerr == EWOULDBLOCK || sockerr == EAGAIN || sockerr == WSAETIMEDOUT) {
				nBytes = 0;
				continue;
			}
			yang_error("%s, recv returned %d. GetSockError(): %d (%s)",
				__FUNCTION__, nBytes, sockerr, strerror(sockerr));

			break;
		}
		else if (nBytes == 0) {

			break;
		}
		break;
	}
	closesocket(server->m_socket);
	yang_free(buf);

	return NULL;

success:
	closesocket(server->m_socket);
	yang_free(buf);
	return NULL;
}


void YangScreenCaptureWin::pushVideo() {
	m_screenFrame.pts = yang_get_system_time() - m_startTime;
	memcpy(m_yuv + 4, (char*)(&m_bufLen), 4);
	if (m_screenFrame.payload) {
		if (m_yuv) {
			int32_t nBytes = send(m_socket, (char*)m_yuv, m_bufLen, 0);
		}
	}
}

void YangScreenCaptureWin::stop()
{
	m_isLoop = yangfalse;
}

void YangScreenCaptureWin::run()
{
	m_isStart = yangtrue;
	startLoop();
	m_isStart = yangfalse;
}

void YangScreenCaptureWin::initScreen() {

	if (g_screenCapture3 == NULL) {

#if Yang_Enable_ScreenCaptureDXGI2
		CoInitialize(NULL);
		g_screenCapture3 = new YangDXGIManager();

#else
		g_screenCapture3 = new YangGDIManager(m_pixelBits);
#endif
	}
}


int32_t YangScreenCaptureWin::startLoop() {
	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(Yang_TcpServer_Port);

	serverAddress.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int32_t m_pixelBytes = m_pixelBits / 8;
	Sleep(2000);
	int count = 0;
	while (count < 6) {
		if (connect(m_socket, (struct sockaddr*)&serverAddress,
			sizeof(struct sockaddr)) == -1) {
			if (count > 4) {
				closesocket(m_socket);
				return yang_error_wrap(ERROR_SOCKET, "tcp connect socket error(%d)",
					WSAGetLastError());
			}
		}
		else {
			break;
		}
		Sleep(2000);
		count++;
	}
	m_isLoop = yangtrue;

	CreateThread(NULL, 0, yang_run_loginserver_thread, this, 0, NULL);
	while (m_isLoop) {
		if (YangScreen::isWinlogin()) break;
		Sleep(1000);
	}

	YangScreen::toCurrent(true);
	initScreen();
	g_screenCapture3->init();
	yang_trace("\nscreen capture init success");

#if Yang_Enable_ScreenCaptureDXGI2
	//CSMonitor1 CSDesktop
	g_screenCapture3->SetCaptureSource(CSDesktop);
#endif
	RECT rcDim;
	g_screenCapture3->GetOutputRect(rcDim);

	int dwWidth = rcDim.right - rcDim.left;
	int dwHeight = rcDim.bottom - rcDim.top;
	
	m_context->avinfo.video.width = dwWidth;
	m_context->avinfo.video.outWidth = dwWidth;
	m_context->avinfo.video.height = dwHeight;
	m_context->avinfo.video.outHeight = dwHeight;

	yang_trace("\ndwWidth=%d dwHeight=%d,bits=%d\n", dwWidth, dwHeight,m_pixelBits);

	DWORD dwBufSize = dwWidth * dwHeight * 4;
	int yuvLen = dwWidth * dwHeight * 3 / 2;
	m_bufLen = yuvLen;
	YangYuvConvert yuv;
	uint8_t* pBuf = new uint8_t[dwWidth * dwHeight * 5];
	uint8_t* dstBuf = new uint8_t[yuvLen];
	dstBuf[0] = 0x02;
	dstBuf[1] = 0x00;
	dstBuf[2] = 0x00;
	dstBuf[3] = 0x02;
	uint8_t* dst = dstBuf + 8;
	m_yuv = dstBuf;

	memset(&m_screenFrame, 0, sizeof(YangFrame));
	m_startTime = 0;
	m_screenFrame.payload = dst;
	m_screenFrame.nb = dwWidth * dwHeight * 3 / 2;
	if (m_context->avinfo.video.videoEncoderFormat == YangArgb) {
		m_screenFrame.nb = dwWidth * dwHeight * 4;
		m_screenFrame.payload = pBuf;
	}
	HRESULT hr = 0;	

	while (m_isLoop) {
		Sleep(100);
		if (!YangScreen::isWinlogin()) continue;
		g_screenCapture3->m_isDrawmouse = m_isDrawmouse;
		hr = g_screenCapture3->GetOutputBits(pBuf, rcDim);
		if (FAILED(hr)) {
			if (hr == DXGI_ERROR_WAIT_TIMEOUT) {
				//yang_error("GetOutputBits failed because DXGI_ERROR_WAIT_TIMEOUT\n");
			}
			else if (hr == DXGI_ERROR_ACCESS_LOST) {
				yang_error("GetOutputBits failed because DXGI_ERROR_ACCESS_LOST\n");
#if !Yang_Enable_ScreenCaptureDXGI2
				g_screenCapture3->init2();
#endif
			}
			else
				yang_error("GetOutputBits failed with hr=0x%08x\n", hr);

		}
		else {
			if (m_startTime == 0)
				m_startTime = yang_get_system_time();

			if (m_context->avinfo.video.videoEncoderFormat == YangI420) {
#if Yang_Enable_ScreenCaptureDXGI2
				yuv.bgratoI420(pBuf, dst, dwWidth, dwHeight);
				//yuv.rgbatoI420(pBuf, dst, dwWidth, dwHeight);
#else
				if (m_pixelBits == 32) {
					
					yuv.bgratoI420(pBuf, dst, dwWidth, dwHeight);
				}
				else {
					yuv.rgb24toI420(pBuf, dst, dwWidth, dwHeight);
				}
				
#endif
			}
			
			pushVideo();

		}

	}
	m_screenFrame.payload = NULL;
	yang_deleteA(pBuf);
	yang_deleteA(dstBuf);
	yang_delete(g_screenCapture3);


	/*if (!CloseDesktop(inputDesk))
	{
		yang_error("CloseDesktop inputDesk error(%lu)", GetLastError());
		return 1;
	}*/

#if Yang_Enable_ScreenCaptureDXGI2
	CoUninitialize();
#endif
	return Yang_Ok;
}
