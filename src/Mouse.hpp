

#ifndef MouseCursor_HPP
#define MouseCursor_HPP

#include "AllegroAndWindows.hpp"

#include "Mutex.hpp"

#include "DIBbuffer.hpp"


#include <vector>
using std::vector;




/* Pass -1 for w or h to use default size */
ALLEGRO_BITMAP* CreateMouseImage(int w , int h , bool active);



bool MouseWindowCallback(ALLEGRO_DISPLAY* d , UINT msg , WPARAM wp , LPARAM lp , LRESULT* lr , void* mouse);



class Mouse {

public :
   // Window info
   ALLEGRO_DISPLAY* display;
   HWND window;
   HDC winhdc;
   
   // Access control
///   Mutex mutex;
   
   // Mouse info
   HANDLE device_handle;
   
   ALLEGRO_BITMAP* ms_image;
   
   DIBbuffer dib_buffer;
   
   int x;
   int y;
   int w;
   int h;
   
   int focusx;
   int focusy;
   
   bool active;
   bool ready;
   
   COLORREF trans_color;
   
   
   
   void QueuePaintMessage();
   void DrawMouseToDIB();
   
   
public :
   
//   Mouse();
Mouse() :
      display(0),
      window(0),
      winhdc(0),
///      mutex(),
      device_handle(0),
      ms_image(0),
      dib_buffer(),
      x(0),
      y(0),
      w(0),
      h(0),
      focusx(0),
      focusy(0),
      active(false),
      ready(false),
      trans_color(RGB(255,255,255))
{
   
}

///   ~Mouse();
~Mouse() {
   Free();
}


   void Free();
   bool Create(int width , int height);
///   bool Create(ALLEGRO_BITMAP* mouse_image);

   void SetHandle(HANDLE hDevice);
   bool SetImage(ALLEGRO_BITMAP* mouse_image);

   void SetPos(int newx , int newy);
   void MoveBy(int dx , int dy);
   void Draw();



   void HandleRawInput(RAWINPUT rawinput);
   
};






class MouseController {
public:
   map<HANDLE , Mouse*> dev_map;
   
   ALLEGRO_BITMAP* ms_enabled_image;
   ALLEGRO_BITMAP* ms_disabled_image;
   
   bool enabled;
   


   void DestroyMouse(HANDLE hDevice);
   bool CreateMouse(HANDLE hDevice);

   void DestroyMice();

public :
   MouseController();
   
   ~MouseController();
   
   void FreeMouseImages();
   bool CreateMouseImages();

   
   void HandleRawInput(RAWINPUT rawinput);
   
   void Draw();
   
   void ToggleMiceEnabled();
   
   void GetMiceWindows(vector<HWND>* micevec);
   
};





#endif // MouseCursor_HPP

