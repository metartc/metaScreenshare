#include "YangGDIManager.h"
#include <yangutil/sys/YangLog.h>
#ifdef _MSC_VER
#include <comdef.h>
#include <gdiplus.h>
#pragma comment (lib,"D3D11.lib")
#pragma comment (lib,"dxgi.lib")
#pragma comment (lib,"gdiplus.lib")
#pragma comment (lib,"gdi32.lib")

POINT pt;
HCURSOR hCursor = NULL ;
HWND hwndPt;
DWORD  dwTid=0;
DWORD   dwCurTid =0;

#define MIRROR_DRIVER_NAME      "RDP Encoder Mirror Driver"
YangGDIManager::YangGDIManager(int32_t pixelBits)
{
    pWnd=NULL;
    m_screenDC=NULL;
    m_compatibleDC=NULL;
    m_compatibleBitmap=NULL;
    m_gdiplusToken=NULL;
    outWidth=1280;outHeight=720;
    lineminus=0;
    linebytes=0;
    m_pixels=NULL;
    m_pixelBits = pixelBits;
    m_pixelBytes = m_pixelBits >> 3;
}

YangGDIManager::~YangGDIManager(void)
{


 if(m_compatibleDC) DeleteObject(m_compatibleDC);

    if(m_screenDC) DeleteDC(m_screenDC);//ReleaseDC(NULL,m_desktopDC);
    if(m_compatibleBitmap) DeleteObject(m_compatibleBitmap);
    if(m_gdiplusToken) Gdiplus::GdiplusShutdown(m_gdiplusToken);
    pWnd=NULL;
    m_screenDC=NULL;
    m_compatibleDC=NULL;
    m_compatibleBitmap=NULL;
    yang_deleteA(m_pixels);
    m_isDrawmouse=false;
}

CURSORINFO cCursorInfo;

void YangGDIManager::init(){
    m_pixelBytes = m_pixelBits >> 3;
    memset(&cCursorInfo, 0, sizeof(cCursorInfo));
    cCursorInfo.cbSize = sizeof(cCursorInfo);

   RECT rect;

   Gdiplus::GdiplusStartupInput gdiplusStartupInput;
   Gdiplus::Status st = Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

    m_screenDC= CreateDCA("DISPLAY", "", NULL, NULL);
    m_compatibleDC=CreateCompatibleDC(m_screenDC);
    GetClipBox(m_screenDC,&rect);
    width = rect.right-rect.left;//GetDeviceCaps(m_screenDC, HORZRES);
    height= rect.bottom-rect.top;//GetDeviceCaps(m_screenDC, VERTRES);
   // width  = ::GetSystemMetrics(SM_CXSCREEN);
   // height = ::GetSystemMetrics(SM_CYSCREEN);

    yang_trace("\nwidth==%d,height==%d",width,height);
    outWidth=width;
    outHeight=height;
    inLen=width*height*2;
    outLen=outWidth*outHeight*2;
    linebytes= m_pixelBytes*width*height;

    m_compatibleBitmap=CreateCompatibleBitmap(m_screenDC,width, height);
    SelectObject(m_compatibleDC,m_compatibleBitmap);




    m_pixels = new uint8_t[width * height*5];
    m_bmi.bmiHeader.biSize = sizeof(m_bmi.bmiHeader);
    m_bmi.bmiHeader.biWidth = width;
    m_bmi.bmiHeader.biHeight = height;
    m_bmi.bmiHeader.biPlanes = 1;
    m_bmi.bmiHeader.biBitCount = m_pixelBits;
    m_bmi.bmiHeader.biCompression = BI_RGB;
    linebytes=outWidth*outHeight*2;
    lineminus=4-((width* m_pixelBytes)>>3)&3;
   
    //hDC=::GetDC(NULL);
    //hMemDC=CreateCompatibleDC(hDC);
}
void YangGDIManager::init2(){

}

HRESULT YangGDIManager::GetOutputRect(RECT& rc){
    rc.left=0;
    rc.top=0;
    rc.right  = width;
    rc.bottom = height;
    return S_OK;
}

HRESULT YangGDIManager::GetOutputBits(BYTE* pBits, RECT& rcDest){
  
    //BitBlt(hDesktopCompatibleDC ,0,0,width ,height,hDesktopDC,0,0,SRCCOPY|CAPTUREBLT);
    BitBlt(m_compatibleDC ,0,0,width ,height,m_screenDC,0,0,SRCCOPY);

 

   if(m_isDrawmouse)  {
       GetCursorPos(&pt);

       GetCursorInfo(&cCursorInfo);
       hCursor = cCursorInfo.hCursor;
       DrawIconEx(m_compatibleDC, pt.x, pt.y, hCursor, 0, 0, 0, NULL, DI_NORMAL | DI_COMPAT | DI_DEFAULTSIZE);
    }

    GetDIBits(m_compatibleDC,
              m_compatibleBitmap,
              0,
              height,
              m_pixels,
              &m_bmi,
              DIB_RGB_COLORS);

    for(int i=0;i<height;i++){
        memcpy(pBits+width* m_pixelBytes *i,m_pixels+width* m_pixelBytes *(height-1-i),width* m_pixelBytes);
    }

    return S_OK;

}





#endif
