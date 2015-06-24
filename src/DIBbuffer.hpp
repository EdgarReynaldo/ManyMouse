


#ifndef DIBbuffer_HPP
#define DIBbuffer_HPP



#include "AllegroAndWindows.hpp"



class DIBbuffer {
   HWND win_handle;
   HDC winDC;
   HDC memDC;
   
   HBITMAP hbm_DIBbuffer;
   BITMAPINFO bm_info;
   void* hbm_DIBdata;/// This has proven to be useless so far - don't know what its good for
   
   HGDIOBJ oldGDIobj;
   
   bool ready;
   
   /// Make copy constructor and assigment operator private to disallow copying
   DIBbuffer(const DIBbuffer& dib) {(void)dib;}
   const DIBbuffer& operator=(const DIBbuffer& rhs) {(void)rhs;return *this;}

   BYTE* GetDataByte(int x , int y);// access to hbm_DIBdata array

   friend void DrawBitmapToDIB(const DIBbuffer& dib_buf , ALLEGRO_BITMAP* bmp);

public :
   DIBbuffer();
   ~DIBbuffer();

   void Free();
   bool Create(HWND window_handle);// Creates a DIB compatible with this window of the same size

   bool GetBitmapInfo(BITMAPINFO* pbi , HWND handle);

   void DrawBufferToWindowDC();

   HDC GetBufferDC() {return memDC;}
   
   void ClearToColor(COLORREF);
   void ClearToColor(int red , int green , int blue , int alpha);

   void FillAreaRect(float fx , float fy , float fw , float fh , COLORREF c);

   bool Ready() {return ready;}
   
   void Test();

   void SetXYRGB(int x , int y , char r , char g , char b);
   
   void Flush();// calls GdiFlush(). Useful?

};






#endif // DIBbuffer_HPP



