//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YangGDIManager_H_
#define YangGDIManager_H_
#ifdef _MSC_VER
#include <yangutil/yangtype.h>
#include <windows.h>
class YangGDIManager
{
public:
    YangGDIManager(int32_t pixelBits);
    ~YangGDIManager(void);
    void captureScreen();
    void init();
    void init2();
    void trans();

    HRESULT GetOutputRect(RECT& rc);
    HRESULT GetOutputBits(BYTE* pBits, RECT& rcDest);

    bool m_isDrawmouse;

    


private:
    HWND pWnd;
    HDC m_screenDC;
    HDC m_compatibleDC;
    HBITMAP m_compatibleBitmap;

    BITMAPINFO m_bmi;

    int width,height,outWidth,outHeight;
    int linebytes,lineminus;
    int inLen,outLen;
    int32_t m_pixelBits;
    int32_t m_pixelBytes;
    uint8_t* m_pixels;
    ULONG_PTR m_gdiplusToken;

};
#endif
#endif
