

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

///   bool active;
   bool ready;

   int ldx;// last delta x
   int ldy;// last delta y

   //RECT bounds;

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
///      active(false),
      ready(false),
      ldx(0),
      ldy(0)
{
  /* bounds.left = 0;
   bounds.right = 0;
   bounds.top = 0;
   bounds.bottom = 0;
*/}

   ~Mouse();

   void CloseOurWindow();
   bool CreateOurWindow(ALLEGRO_BITMAP* mouse_image);

   void SetHandle(HANDLE hDevice);
   bool SetImage(ALLEGRO_BITMAP* mouse_image);

/*   void SetBounds(RECT r);
   void SetBounds(RECT r){
      bounds = r;
      SetPos(x,y);
   }*/

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



class MouseController {
public:

   MouseTracker mouse_tracker;

   ALLEGRO_BITMAP* ms_enabled_image;
   ALLEGRO_BITMAP* ms_disabled_image;

   bool enabled;

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

   void ToggleMiceEnabled();

   void GetMiceWindows(vector<HWND>* winvec);
   vector<Mouse*> GetMice();

   void SetWindowHandler(WindowHandler* handler);

};





#endif // MouseCursor_HPP

