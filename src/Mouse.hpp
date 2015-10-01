

#ifndef MouseCursor_HPP
#define MouseCursor_HPP



#include "AllegroAndWindows.hpp"
#include "DIBbuffer.hpp"
#include "Mutex.hpp"
///#include "WindowHandler.hpp"
#include "TransparentWindow.hpp"

class WindowHandler;

#include <vector>
using std::vector;



#define NUM_MOUSE_BUTTONS 5


enum NORMAL_MOUSE_STATE {
   NORMAL_MOUSE_INACTIVE = 0,
   NORMAL_MOUSE_ACTIVE = 1
};

#define NUM_NORMAL_MOUSE_STATES 2

extern const char* normal_mouse_paths[NUM_NORMAL_MOUSE_STATES];

extern ALLEGRO_BITMAP* normal_mouse_images[NUM_NORMAL_MOUSE_STATES];



enum HEAVY_MOUSE_STATE {
   HEAVY_MOUSE_INACTIVE = 0,
   HEAVY_MOUSE_ACTIVE = 1,
   HEAVY_MOUSE_GRABBING = 2,
   HEAVY_MOUSE_NEEDS_HELPER = 3,
   HEAVY_MOUSE_HELPER_READY = 4,
   HEAVY_MOUSE_DRAGGING = 5
};

#define NUM_HEAVY_MOUSE_STATES 6

extern const char* heavy_mouse_paths[NUM_HEAVY_MOUSE_STATES];

extern ALLEGRO_BITMAP* heavy_mouse_images[NUM_HEAVY_MOUSE_STATES];



void FreeMiceImages();

bool LoadMiceImages();


/* Pass -1 for w or h to use default size */
ALLEGRO_BITMAP* CreateMouseImage(int w , int h , bool active);


int FlagsToButtonIndex(USHORT flags , bool* down);





class Mouse {

public :

   TransparentWindow transparent_window;

   // Access control
///   Mutex mutex;

   // Mouse info
   HANDLE device_handle;

   ALLEGRO_BITMAP* ms_image;

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

public :

//   Mouse();
Mouse() :
      transparent_window(),
      device_handle(0),
      ms_image(0),
      x(0),
      y(0),
      focusx(0),
      focusy(0),
      shown(true),
      ready(false),
      ldx(0),
      ldy(0),
      bounds(),
      buttons_down()
{
   bounds.left = 0;
   bounds.right = 0;
   bounds.top = 0;
   bounds.bottom = 0;
   for (unsigned int i = 0 ; i < NUM_MOUSE_BUTTONS ; ++i) {
      buttons_down[i] = false;
   }
}

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
   
   bool GetButtonState(int button) {
      ALLEGRO_ASSERT(button > 0 && button < NUM_MOUSE_BUTTONS + 1);
      return buttons_down[button - 1];
   }
};






#endif // MouseCursor_HPP

