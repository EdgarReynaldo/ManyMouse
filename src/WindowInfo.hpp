

#ifndef WindowInfo_HPP
#define WindowInfo_HPP


#include "AllegroAndWindows.hpp"
///#include "String.hpp"

#include <string>
using std::string;



#include "DllBuild.hpp"



MMDECLSPEC bool RectContains(RECT r , int xpos , int ypos);
MMDECLSPEC string RectString(RECT r);



class MMDECLSPEC WindowInfo {

public :
   HWND hwnd;
   HWND parent;
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
   WindowInfo();
   WindowInfo(HWND window);
   WindowInfo(const WindowInfo& rhs);





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
