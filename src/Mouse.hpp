

#ifndef MouseCursor_HPP
#define MouseCursor_HPP



#include "AllegroAndWindows.hpp"
#include "DIBbuffer.hpp"
#include "Mutex.hpp"
///#include "WindowHandler.hpp"
class WindowHandler;

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
   
   int X() {return x;}
   int Y() {return y;}
   
   
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
///   map<HANDLE , Mouse*> dev_map;
///   map<unsigned int , Mouse*> mouse_id_map;
   
///   MouseIdHelper id_helper;
   
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
   
/*
   int NumMiceActive();
int NumMiceActive() {
   int n = 0;
   map<HWND , Mouse*>::iterator it = dev_map.begin();
   while (it != dev_map.end()) {
      ++n;
      ++it;
   }
   return n;
}
   Mouse* GetMouse(int mindex);
   Mouse* GetMouse(int mindex) {
      
   }
*/
};





#endif // MouseCursor_HPP

