


#ifndef DIBbuffer_HPP
#define DIBbuffer_HPP



#include "AllegroAndWindows.hpp"

#include "DllBuild.hpp"



class MMDECLSPEC DIBbuffer {
   HWND win_handle;
   HDC winDC;
   HDC memDC;
   
   HBITMAP hbm_DIBbuffer;
   BITMAPINFO bm_info;
   void* hbm_DIBdata;
   
   HGDIOBJ oldGDIobj;
   
   bool ready;
   
   bool draw_with_alpha;
   
   
   
   /// Make copy constructor and assigment operator private to disallow copying
   DIBbuffer(const DIBbuffer& dib) {(void)dib;}
   const DIBbuffer& operator=(const DIBbuffer& rhs) {(void)rhs;return *this;}

   BYTE* GetDataByte(int x , int y);// access to hbm_DIBdata array

   friend MMDECLSPEC void DrawBitmapToDIB(const DIBbuffer& dib_buf , ALLEGRO_BITMAP* bmp);

public :
   DIBbuffer();
   ~DIBbuffer();

   void Free();
   bool Create(HWND window_handle);// Creates a DIB compatible with this window of the same size

   bool GetBitmapInfo(BITMAPINFO* pbi , HWND handle);

   void BlitBufferToWindowDC();
   void BlendBufferToWindowDC();

   HDC GetBufferDC() {return memDC;}
   
   void ClearToColor(COLORREF);
   void ClearToColor(int red , int green , int blue , int alpha);

   void FillAreaRect(float fx , float fy , float fw , float fh , COLORREF c);

   bool Ready() {return ready;}
   
   void Test();

   void SetXYRGBA(int x , int y , char r , char g , char b , char a);
   
   void UseAlphaDrawing(bool use_alpha) {draw_with_alpha = use_alpha;}
   
   void Flush();// calls GdiFlush(). Useful?

};






#endif // DIBbuffer_HPP



