

#ifndef MouseCursor_HPP
#define MouseCursor_HPP



#include "TransparentWindow.hpp"
//#include "Mutex.hpp"


#define NUM_MOUSE_BUTTONS 5


#include "DllBuild.hpp"



MMDECLSPEC int FlagsToButtonIndex(USHORT flags , bool* down);



struct ALLEGRO_BITMAP;
struct ALLEGRO_FONT;


class MMDECLSPEC Mouse {

public :

   TransparentWindow transparent_window;

   // Access control
///   Mutex mutex;

   // Mouse info
   HANDLE device_handle;

   int mouse_device_number;

   int x;
   int y;
   int focusx;
   int focusy;

   bool shown;
   bool ready;

   int ldx;// last delta x
   int ldy;// last delta y

   RECT bounds;
   
   bool buttons_down[NUM_MOUSE_BUTTONS];
   
   ALLEGRO_FONT* mouse_font;
   

public :

   Mouse();

   ~Mouse();

   void CloseOurWindow();
   bool CreateOurWindow(ALLEGRO_BITMAP* mouse_image);

   void SetHandle(HANDLE hDevice);
   bool SetImage(ALLEGRO_BITMAP* mouse_image);

///   void SetBounds(RECT r);
   void SetBounds(RECT r){
      bounds = r;
      SetPos(x,y);
   }

   void SetPos(int newx , int newy);
   void MoveBy(int dx , int dy);
   void Draw();

   void HandleRawInput(RAWINPUT rawinput);

   int X() {return x;}
   int Y() {return y;}
   int LastDeltaX() {return ldx;}
   int LastDeltaY() {return ldy;}

   bool MouseMoved() {return ldx || ldy;}

   HWND GetMouseWindowHandle();

   void BringMouseToFront();
   void ShowMouse(bool show_mouse);
   
   void SetDeviceNumber(int dev_num);

   int MouseDeviceNumber() {return mouse_device_number;}

   bool GetButtonState(int button);
   
//   void SetTransColor(int r , int g , int b);
   void SetTransColor(int r , int g , int b) {
       transparent_window.SetTransColor(r,g,b);
   }
   
   ALLEGRO_FONT* GetMouseFont() {return mouse_font;}
   
};






#endif // MouseCursor_HPP

