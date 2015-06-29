


#include "DIBbuffer.hpp"
#include "VisualLogger.hpp"

using namespace ManyMouse;

/*   
HBITMAP CreateDIBSection(
  _In_   HDC        hdc,
  _In_   const BITMAPINFO *pbmi,
  _In_   UINT       iUsage,
  _Out_  VOID       **ppvBits,
  _In_   HANDLE     hSection,
  _In_   DWORD      dwOffset
);
typedef struct tagBITMAPINFOHEADER {
  DWORD biSize;
  LONG  biWidth;
  LONG  biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG  biXPelsPerMeter;
  LONG  biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;
typedef struct tagBITMAPINFO {
  BITMAPINFOHEADER bmiHeader;
  RGBQUAD          bmiColors[1];
} BITMAPINFO, *PBITMAPINFO;
*/





BYTE* DIBbuffer::GetDataByte(int x , int y) {
   if (!ready || !hbm_DIBdata) {
      return 0;
   }
   int w = bm_info.bmiHeader.biWidth;
   int h = bm_info.bmiHeader.biHeight;
   if (x < 0 || x >= w || y < 0 || y >= h) {
      return 0;
   }
   
   int pitch = 4*w;
   return &((BYTE*)hbm_DIBdata)[y*pitch];
   
//   int pitch = 3*w + ((3*w)%4?(4-((3*w)%4)):0);
//   return &((BYTE*)hbm_DIBdata)[y*pitch + 3*x];
}



DIBbuffer::DIBbuffer() :
   win_handle(0),
   winDC(0),
   memDC(0),
   hbm_DIBbuffer(0),
   bm_info(),
   hbm_DIBdata(0),
   ready(false)
{
   memset(&bm_info , 0 , sizeof(BITMAPINFO));
}



DIBbuffer::~DIBbuffer() {
   Free();
}


void DIBbuffer::Free() {
   
   if (oldGDIobj) {
      // put back the old object if there was one
      SelectObject(memDC , oldGDIobj);
      oldGDIobj = 0;
   }
   
   
   if (hbm_DIBbuffer) {
      DeleteObject(hbm_DIBbuffer);
      hbm_DIBbuffer = 0;
   }
   
   if (memDC) {
      DeleteDC(memDC);
      memDC = 0;
   }
   
   if (win_handle && winDC) {
      ReleaseDC(win_handle , winDC);
      winDC = 0;
   }
   
   memset(&bm_info , 0 , sizeof(BITMAPINFO));
   
   
   ready = false;
   
}


bool DIBbuffer::Create(HWND window_handle) {
   if (!window_handle) {
      return false;
   }
   
   Free();
   
   win_handle = window_handle;
   
   winDC = GetDC(window_handle);
   
   memDC = CreateCompatibleDC(winDC);

   if (!winDC ||
       !memDC ||
       !GetBitmapInfo(&bm_info , win_handle)) {
      Free();
      if (!winDC) {
         log.Log("DIBbuffer::Create - Could not get window DC from hwnd %p\n" , window_handle);
      }
      else if (!memDC) {
         log.Log("DIBbuffer::Create - Could create compatible DC from winDC %p\n" , winDC);
      }
      else { // GetBitmapInfo failed
         log.Log("DIBbuffer::Create - Could not get bitmap info from hwnd %p\n" , window_handle);
      }
      return false;
   }

   hbm_DIBbuffer = CreateDIBSection(memDC , &bm_info , DIB_RGB_COLORS , &hbm_DIBdata , 0 , 0);
   
   if (!hbm_DIBbuffer || !hbm_DIBdata) {
      if (!hbm_DIBbuffer) {
         log.Log("DIBbuffer::Create - CreateDIBSection failed\n");
      }
      if (!hbm_DIBdata) {
         log.Log("DIBbuffer::Create - CreateDIBSection did not allocate data!\n");
      }
      Free();
      return false;
   }
   
   oldGDIobj = SelectObject(memDC , hbm_DIBbuffer);
   
   if (oldGDIobj == HGDI_ERROR) {
      log.Log("HGDI_ERROR occurred while selecting object %p\n" , memDC);
      Free();
      return false;
   }
   
   ready = true;
   
   Test();
   
   return true;
}



bool DIBbuffer::GetBitmapInfo(BITMAPINFO* pbi , HWND handle) {
   // Gets bitmap info suitable for creating a DIB the same size as the window specified by 'handle'.
   if (!pbi || !handle) {
      if (!pbi) {
         log.Log("DIBbuffer::GetBitmapInfo - pbi is NULL.\n");
      }
      if (!handle) {
         log.Log("DIBbuffer::GetBitmapInfo - handle is NULL.\n");
      }
      return false;
   }
   
   memset(pbi , 0 , sizeof(*pbi));
   
   RECT clrect;
   if (!GetClientRect(handle , &clrect)) {
      log.Log("DIBbuffer::GetBitmapInfo - failed to get client rect from hwnd %p\n" , handle);
      return false;
   }
   
   int w = clrect.right - clrect.left;
   int h = clrect.bottom - clrect.top;
   
   BITMAPINFOHEADER* phdr = &(pbi->bmiHeader);
   phdr->biSize = sizeof(BITMAPINFOHEADER);
   phdr->biWidth = w;
   phdr->biHeight = -h;// negative height indicates this is a top down bitmap
   phdr->biPlanes = 1;
//   phdr->biBitCount = 24;// 32 does no good - alpha is unused?
   phdr->biBitCount = 32;
   phdr->biCompression = BI_RGB;
   phdr->biSizeImage = 0;// okay for BI_RGB bitmaps
   phdr->biXPelsPerMeter = 0;//?
   phdr->biYPelsPerMeter = 0;//?
   phdr->biClrUsed = 0;
   phdr->biClrImportant = 0;//?
   
   return true;
}



void DIBbuffer::BlitBufferToWindowDC() {
   if (!ready) {
      log.Log("DIBbuffer::BlitBufferToWindowDC - not ready.\n");
      return;
   }
//**   
   // if ready, everything is set up, and our DIB is already selected into the memory DC
   
   RECT clrect;
   
   GetClientRect(win_handle , &clrect);
   
   int dx = clrect.left;
   int dy = clrect.top;
   int dw = clrect.right - clrect.left;
   int dh = clrect.bottom - clrect.top;

   BitBlt(winDC , dx , dy , dw , dh , memDC , 0 , 0 , SRCCOPY);
   GdiFlush();


//*/

//   BLENDFUNCTION blend = {AC_SRC_OVER, 0, 127, 0};
   
///   HBRUSH hbr = CreateSolidBrush(RGB(0,0,255));
   

///   SetDCPenColor(memDC, RGB(0,0,255));
///   SetDCBrushColor(memDC, RGB(0,0,255));
///   FillRect(memDC, &r, hbr);

/*
BOOL AlphaBlend(
  _In_  HDC           hdcDest,
  _In_  int           xoriginDest,
  _In_  int           yoriginDest,
  _In_  int           wDest,
  _In_  int           hDest,
  _In_  HDC           hdcSrc,
  _In_  int           xoriginSrc,
  _In_  int           yoriginSrc,
  _In_  int           wSrc,
  _In_  int           hSrc,
  _In_  BLENDFUNCTION ftn
);*/

/**   

   RECT sr;
   sr.left = 0;
   sr.right = bm_info.bmiHeader.biWidth;
   sr.top = 0;
   sr.bottom = abs(bm_info.bmiHeader.biHeight);
   RECT dr;
   GetClientRect(win_handle , &dr);
   int dx = dr.left;
   int dy = dr.top;
   int dw = dr.right - dr.left;
   int dh = dr.bottom - dr.top;


//typedef struct _BLENDFUNCTION {
//  BYTE BlendOp;
//  BYTE BlendFlags;
//  BYTE SourceConstantAlpha;
//  BYTE AlphaFormat;
//} BLENDFUNCTION, *PBLENDFUNCTION, *LPBLENDFUNCTION;

   BLENDFUNCTION blend = {AC_SRC_OVER, 0, 64, 0};//AC_SRC_ALPHA};

   blend.BlendOp = AC_SRC_OVER;
   blend.BlendFlags = 0;
   blend.SourceConstantAlpha = 255;
   blend.AlphaFormat = AC_SRC_ALPHA;

   GdiFlush();
   
///   log.Log("Using AlphaBlend");
///   if (!AlphaBlend(winDC, dr.left, dr.top, dr.right - dr.left, dr.bottom - dr.top,  memDC, sr.left, sr.top, sr.right, sr.bottom, blend)) {
///      log.Log("AlphaBlend failed. GetLastError reports %d.\n" , GetLastError());
///   }
   
   GdiFlush();
//*/

///   DeleteObject(hbr);

}

//-lmsimg32
//-lwin32k

//*

void DIBbuffer::BlendBufferToWindowDC() {

//   HDC winhdc = GetDC(window);
   
/*
BOOL WINAPI UpdateLayeredWindow(
  _In_     HWND          hwnd,
  _In_opt_ HDC           hdcDst,
  _In_opt_ POINT         *pptDst,
  _In_opt_ SIZE          *psize,
  _In_opt_ HDC           hdcSrc,
  _In_opt_ POINT         *pptSrc,
  _In_     COLORREF      crKey,
  _In_opt_ BLENDFUNCTION *pblend,
  _In_     DWORD         dwFlags
);
*/
/**
//   HDC screenDC = GetDC(NULL);

   POINT pd;
   pd.x = 0;
   pd.y = 0;
   POINT ps;
   ps.x = 0;
   ps.y = 0;
   SIZE ssz;
   ssz.cx = bm_info.bmiHeader.biWidth;
   ssz.cy = abs(bm_info.bmiHeader.biHeight);
   
   BLENDFUNCTION blend;
   blend.BlendOp = AC_SRC_OVER;
   blend.BlendFlags = 0;
   blend.SourceConstantAlpha = 255;
   blend.AlphaFormat = AC_SRC_ALPHA;
   
   if (!UpdateLayeredWindow(win_handle , winDC , &pd , &ssz , memDC , &ps , RGB(0,0,0) , &blend , ULW_ALPHA)) {
      log.Log("Failed to update layered window. GetLastError reports %d.\n" , GetLastError());
   }

//   ReleaseDC(NULL , screenDC);
//*/

   RECT sr;
   sr.left = 0;
   sr.right = bm_info.bmiHeader.biWidth;
   sr.top = 0;
   sr.bottom = abs(bm_info.bmiHeader.biHeight);
   RECT dr;
   GetClientRect(win_handle , &dr);
   int dx = dr.left;
   int dy = dr.top;
   int dw = dr.right - dr.left;
   int dh = dr.bottom - dr.top;


   if (!draw_with_alpha) {   
      BitBlt(winDC , dx , dy , dw , dh , memDC , 0 , 0 , SRCCOPY);
   }
   else {

   //typedef struct _BLENDFUNCTION {
   //  BYTE BlendOp;
   //  BYTE BlendFlags;
   //  BYTE SourceConstantAlpha;
   //  BYTE AlphaFormat;
   //} BLENDFUNCTION, *PBLENDFUNCTION, *LPBLENDFUNCTION;

      BLENDFUNCTION blend = {AC_SRC_OVER, 0, 64, 0};//AC_SRC_ALPHA};

      blend.BlendOp = AC_SRC_OVER;
      blend.BlendFlags = 0;
      blend.SourceConstantAlpha = 64;
      blend.AlphaFormat = AC_SRC_ALPHA;

      log.Log("Using AlphaBlend");
      if (!AlphaBlend(winDC, dr.left, dr.top, dr.right - dr.left, dr.bottom - dr.top,  memDC, sr.left, sr.top, sr.right, sr.bottom, blend)) {
         log.Log("AlphaBlend failed. GetLastError reports %d.\n" , GetLastError());
      }
   }
   GdiFlush();
//}
}




void DIBbuffer::ClearToColor(COLORREF c) {
   
   if (!ready) {return;}
   
   HBRUSH hbr = CreateSolidBrush(c);
   if (!hbr) {return;}
   

   RECT r;
   GetClientRect(win_handle , &r);

   
   FillRect(memDC , &r , hbr);
   
   DeleteObject(hbr);
}
//*/



void DIBbuffer::ClearToColor(int red , int green , int blue , int alpha) {
   
   if (!ready) {
      log.Log("DIBbuffer::ClearToColor(r,g,b,a) - not ready.\n");
      return;
   }
   
   int color = (((alpha & 0xff) << 24) |
               ((red & 0xff) << 16) |
               ((green & 0xff) << 8) |
               ((blue & 0xff) << 0));
   
   int pitch = bm_info.bmiHeader.biWidth;
   int* dat = (int*)hbm_DIBdata;
   /// data is ARGB
   for (int y = 0 ; y < bm_info.bmiHeader.biHeight ; ++y) {
      for (int x = 0 ; x < bm_info.bmiHeader.biWidth ; ++x) {
         dat[x] = color;
      }
      dat += pitch;
   }
   
/*
   if (!ready) {return;}
   
   HBRUSH hbr = CreateSolidBrush(c);
   if (!hbr) {return;}
   

   RECT r;
   GetClientRect(win_handle , &r);

   
   FillRect(memDC , &r , hbr);
   
   DeleteObject(hbr);
*/
}



void DIBbuffer::FillAreaRect(float fx , float fy , float fw , float fh , COLORREF c) {
   if (!ready) {return;}
   
   HBRUSH hbr = CreateSolidBrush(c);
   if (!hbr) {return;}

   RECT r1,r2;
   
   GetClientRect(win_handle , &r1);
   
   int w = r1.right - r1.left;
   int h = r1.bottom - r1.top;
   
   int x = (int)(r1.left + w*fx);
   int y = (int)(r1.top + h*fy);
   
   w = (int)(w*fw);
   h = (int)(h*fh);
   
   r2.left = x;
   r2.top = y;
   r2.right = x + w;
   r2.bottom = y + h;
   
   FillRect(memDC , &r2 , hbr);
   
   DeleteObject(hbr);
}




void DIBbuffer::Test() {
   if (!ready) {return;}
   
   int w = bm_info.bmiHeader.biWidth;
   int h = bm_info.bmiHeader.biHeight;

   for (int y = 0 ; y < h ; ++y) {
      for (int x = 0 ; x < w ; ++x) {
         BYTE* b = GetDataByte(x,y);
         
         b[0] = 0xff;
         b[1] = 0xaf;
         b[2] = 0x5f;
         b[3] = 0xff;
      }
   }
   GdiFlush();
   BlitBufferToWindowDC();
}



void DIBbuffer::SetXYRGBA(int x , int y , char r , char g , char b , char a) {
   if (!ready) {return;}
   int* pb = (int*)GetDataByte(x,y);
   if (!pb) {return;}
   
   int color = (((int)a << 24) | ((int)r << 16) | ((int)g << 8) | (int)b);
   *pb = color;
   
}




void DIBbuffer::Flush() {
   GdiFlush();
}


