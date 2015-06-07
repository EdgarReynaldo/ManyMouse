


#include "DIBbuffer.hpp"



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
   
   int pitch = 3*w + ((3*w)%4?(4-((3*w)%4)):0);
   return &((BYTE*)hbm_DIBdata)[y*pitch + 3*x];
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
      return false;
   }

   hbm_DIBbuffer = CreateDIBSection(memDC , &bm_info , DIB_RGB_COLORS , &hbm_DIBdata , 0 , 0);
   
   if (!hbm_DIBbuffer || !hbm_DIBdata) {
      Free();
      return false;
   }
   
   oldGDIobj = SelectObject(memDC , hbm_DIBbuffer);
   
   if (oldGDIobj == HGDI_ERROR) {
      Free();
      return false;
   }
   
   ready = true;
   
   Test();
   
   return true;
}



bool DIBbuffer::GetBitmapInfo(BITMAPINFO* pbi , HWND handle) {
   // Gets bitmap info suitable for creating a DIB the same size as the window specified by 'handle'.
   if (!pbi || !handle) {return false;}
   
   memset(pbi , 0 , sizeof(*pbi));
   
   RECT clrect;
   if (!GetClientRect(handle , &clrect)) {return false;}
   
   int w = clrect.right - clrect.left;
   int h = clrect.bottom - clrect.top;
   
   BITMAPINFOHEADER* phdr = &(pbi->bmiHeader);
   phdr->biSize = sizeof(BITMAPINFOHEADER);
   phdr->biWidth = w;
   phdr->biHeight = -h;
   phdr->biPlanes = 1;
   phdr->biBitCount = 24;// 32 does no good - alpha is unused?
   phdr->biCompression = BI_RGB;
   phdr->biSizeImage = 0;// okay for BI_RGB bitmaps
   phdr->biXPelsPerMeter = 0;//?
   phdr->biYPelsPerMeter = 0;//?
   phdr->biClrUsed = 0;
   phdr->biClrImportant = 0;//?
   
   return true;
}



void DIBbuffer::DrawBufferToWindowDC() {
   if (!ready) {return;}
   
   // if ready, everything is set up, and our DIB is already selected into the memory DC
   
   RECT clrect;
   
   GetClientRect(win_handle , &clrect);
   
   int dx = clrect.left;
   int dy = clrect.top;
   int dw = clrect.right - clrect.left;
   int dh = clrect.bottom - clrect.top;

   BitBlt(winDC , dx , dy , dw , dh , memDC , 0 , 0 , SRCCOPY);
   GdiFlush();
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
      }
   }
   GdiFlush();
   DrawBufferToWindowDC();
}



void DIBbuffer::SetXYRGB(int x , int y , char r , char g , char b) {
   if (!ready) {return;}
   BYTE* pb = GetDataByte(x,y);
   if (!pb) {return;}
   pb[0] = r;
   pb[1] = g;
   pb[2] = b;
}




void DIBbuffer::Flush() {
   GdiFlush();
}


