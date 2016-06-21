


#ifndef TransparentWindow_HPP
#define TransparentWindow_HPP


#include "AllegroAndWindows.hpp"
#include "DIBbuffer.hpp"


#include "DllBuild.hpp"


// Private

//bool WindowPainterCallback
//   (ALLEGRO_DISPLAY* display , UINT message , WPARAM wparam , LPARAM lparam , LRESULT* lresult , void* data);



class MMDECLSPEC TransparentWindow {
   
private :
   int x;
   int y;
   int w;
   int h;
   ALLEGRO_DISPLAY* display;
   HWND window;
//   HDC winhdc;
   
//   WINDOW_CALLBACK callback;
//   void* callback_data;
   
   DIBbuffer dib_buffer;
   
   COLORREF trans_color;
   
   ALLEGRO_BITMAP* image;
   
   bool ready;
   
   bool draw_with_alpha;
   
   /// Give the callback access to our data
   friend MMDECLSPEC bool WindowPainterCallback
   (ALLEGRO_DISPLAY* display , UINT message , WPARAM wparam , LPARAM lparam , LRESULT* lresult , void* data);

   /// Make copy constructor and assignment operator private to disallow copying
   TransparentWindow(const TransparentWindow& tw) {(void)tw;}
   const TransparentWindow& operator=(const TransparentWindow& rhs) {(void)rhs;return *this;}


   void DrawImageToDIB();
   void QueuePaintMessage();
   

public :
   
///   TransparentWindow();
   TransparentWindow() :
         x(0),
         y(0),
         w(-1),
         h(-1),
         display(0),
         window(0),
///         callback(0),
///         callback_data(0),
//         winhdc(0),
         dib_buffer(),
         trans_color(RGB(0,0,0)),
         image(0),
         ready(false),
         draw_with_alpha(false)
   {
      
   }

   ~TransparentWindow();
   
   void CloseTheWindow();
   
   bool CreateTheWindow(ALLEGRO_BITMAP* img , int width = -1 , int height = -1);

   void PaintTheWindow();

   bool SetWindowImage(ALLEGRO_BITMAP* img);

   void SetWindowPosition(int xpos , int ypos);

   void SetWindowTitle(const char* title);

   bool AddWindowCallback(WINDOW_CALLBACK cb , void* cb_data);

   void UseAlphaDrawing(bool use_alpha);

   ALLEGRO_DISPLAY* GetAllegroDisplay();

   HWND GetWindowHandle();
   
   bool Ready();
   
//   void SetTransColor(int r , int g , int b);
   void SetTransColor(int r , int g , int b) {
       trans_color = RGB(r,g,b);
   }
   
};

#endif // TransparentWindow_HPP
