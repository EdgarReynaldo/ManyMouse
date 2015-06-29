

#ifndef WindowInfo_HPP
#define WindowInfo_HPP


#include "AllegroAndWindows.hpp"
///#include "String.hpp"

#include <string>
using std::string;



bool RectContains(RECT r , int xpos , int ypos);
string RectString(RECT r);



class WindowInfo {

public :
   HWND hwnd;
   DWORD pid;
   WNDPROC window_process;

   RECT window_rect;
   RECT client_rect;

   string window_title;
   string window_class;
   string window_type;// Mouse / Desktop / Taskbar / Program / Test


   void InitRects();

   void RefreshRects();
   void RefreshWindowTitle();
   void RefreshWindowClassName();
   void RefreshInfo();

public :
//   WindowInfo();
   WindowInfo() :
         hwnd((HWND)0),
         pid(-1),
         window_process((WNDPROC)0),
         window_rect(),
         client_rect(),
         window_title("Unknown window title"),
         window_class("Unknown class name"),
         window_type("Unknown window type")
   {
      InitRects();
   }
   void SetWindowHandle(HWND window);
   void SetWindowTypeString(string type);

   string GetWindowInfoString();

   void RefreshAreas();

   bool WindowContains(int xpos , int ypos);
   bool ClientContains(int xpos , int ypos);

   string WindowTitle() {return window_title;}
   string WindowClass() {return window_class;}
   string WindowType() {return window_type;}
};






#endif // WindowInfo_HPP
