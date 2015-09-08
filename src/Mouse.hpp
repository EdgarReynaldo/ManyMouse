

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




/* Pass -1 for w or h to use default size */
ALLEGRO_BITMAP* CreateMouseImage(int w , int h , bool active);


enum MOUSE_STATE {
   MOUSE_INACTIVE = 0,
   MOUSE_ACTIVE = 1,
   MOUSE_GRABBING = 2,
   MOUSE_NEEDS_HELPER = 3,
   MOUSE_HELPER_READY = 4,
   MOUSE_DRAGGING = 5
};

#define NUM_MOUSE_STATES = 6


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

   MOUSE_STATE mouse_state;
   ALLEGRO_BITMAP* mouse_images[NUM_MOUSE_STATES];

//   double time;
//   double lclicktime;

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
      mouse_state(MOUSE_INACTIVE),
      mouse_images()
//      time(0.0),
//      lclicktime(0.0)
{
   bounds.left = 0;
   bounds.right = 0;
   bounds.top = 0;
   bounds.bottom = 0;
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

/*
   void UpdateClock(double t);
   double GetTimeSinceLastLeftClick();

   void UpdateClock(double t) {
      time = t;
   }

   double GetTimeSinceLastLeftClick() {
      return time - lclicktime;
   }
*/
   void BringMouseToFront();
   void ShowMouse(bool show_mouse);
};



class MouseTrackingInfo {
public :
   bool used;
   unsigned int id;
   HANDLE hdev;
   Mouse* mouse;

//   MouseTrackingInfo();
   MouseTrackingInfo() :
         used(false),
         id(0),
         hdev(0),
         mouse(0)
   {

   }
};



/// MouseTracker is responsible for tracking and destroying mice, but does not create them

class MouseTracker {

private :
   vector<MouseTrackingInfo> mtinfo;

   MouseTracker& operator=(const MouseTracker& mt) {(void)mt;return *this;}// disallow copying
   MouseTracker(const MouseTracker& mt) {(void)mt;}// disallow copy constructor

public :

   MouseTracker();

   unsigned int TrackNewMouse(Mouse* mouse , HANDLE hdev);// adopts ownership of Mouse*
   void StopTrackingMouse(HANDLE hdev);

   ~MouseTracker();

   void CleanUp();

   vector<unsigned int> GetIdsInUse();// duh

   Mouse* GetMouseFromHandle(HANDLE hdev);

   unsigned int NMice();// Number of mice present
   Mouse* GetMouseByIndex(unsigned int index);// call NMice() to find out which indices are valid first

   vector<Mouse*> GetMouseVector();

};


enum MOUSE_STRATEGY {
   MOUSE_STRATEGY_NORMAL = 0,
   MOUSE_STRATEGY_COLLABORATIVE = 1,
   MOUSE_STRATEGY_HEAVYOBJECT = 2
};

enum HEAVY_MOUSE_STATES {
   HEAVY_MOUSE_FREE = 0,
   HEAVY_MOUSE_GRABBING = 1,
   HEAVY_MOUSE_DRAGGING = 2
};

class MouseStrategy {

   MouseTracker* mouse_tracker;
   WindowHandler* window_handler;

public :

   MouseStrategy();
   MouseStrategy() :
         mouse_tracker(0),
         window_handler(0)
   {
   }

   void HandleInput(RAWINPUT input)=0;

   MOUSE_STRATEGY GetStrategy()=0;

};


class NormalMouseStrategy : public MouseStrategy {

public :
   void HandleInput(RAWINPUT input);
   void HandleInput(RAWINPUT input) {

   }

   MOUSE_STRATEGY GetStrategy() {return MOUSE_STRATEGY_NORMAL;}

};



class HeavyMouseStrategy : public MouseStrategy {

   HEAVY_MOUSE_STATE heavy_mouse_state;

public :

   void HandleInput(RAWINPUT input);
   void HandleInput(RAWINPUT input) {
      Mouse* active_mouse = input.hid
      switch (heavy_mouse_state) {
      case HEAVY_MOUSE_FREE :

         break;
      case HEAVY_MOUSE_GRABBING :

         break;
      case HEAVY_MOUSE_DRAGGING :

         break;
      }
   }

   MOUSE_STRATEGY GetStrategy() {return MOUSE_STRATEGY_HEAVYOBJECT;}

};


class MouseController {
public:

   NormalMouseStrategy normal_strategy;
//   CollaborativeMouseStrategy collaborative_strategy;
   HeavyMouseStrategy heavy_strategy;

   MouseStrategy* active_strategy;


   MouseTracker mouse_tracker;

   ALLEGRO_BITMAP* ms_enabled_image;
   ALLEGRO_BITMAP* ms_disabled_image;

   bool mouse_image_enabled;

   bool mice_active;

//   bool mice_enabled;
   bool mice_shown;

   WindowHandler* window_handler;



   void DestroyMouse(HANDLE hDevice);
   bool CreateMouse(HANDLE hDevice);

   void DestroyMice();

   int FlagsToButtonIndex(USHORT flags , bool* down);

public :
   MouseController();

   ~MouseController();

   void FreeMouseImages();
   bool CreateMouseImages();


   void HandleRawInput(RAWINPUT rawinput);

   void Draw();

   void ToggleMouseImage();

   void ActivateMice(bool activate_mice);
   void ShowMice(bool show_mice);


   void GetMiceWindows(vector<HWND>* winvec);
   vector<Mouse*> GetMice();

   void SetWindowHandler(WindowHandler* handler);

   void BringMiceToFront();

   bool IsMouseWindow(HWND hwnd);

};





#endif // MouseCursor_HPP

