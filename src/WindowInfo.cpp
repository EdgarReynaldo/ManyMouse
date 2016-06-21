

#include "WindowInfo.hpp"

#include "String.hpp"



MMDECLSPEC bool RectContains(RECT r , int xpos , int ypos) {
   return ((xpos >= r.left) && (ypos >= r.top) && (xpos < r.right) && (ypos < r.bottom));
}



MMDECLSPEC string RectString(RECT rect) {
   int l = rect.left;
   int r = rect.right;
   int t = rect.top;
   int b = rect.bottom;
   int w = r - l;
   int h = b - t;
   return StringPrintF("%d,%d : %d x %d" , l , r , w , h);
}



MMDECLSPEC WindowInfo::WindowInfo() :
      hwnd((HWND)0),
      parent((HWND)0),
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


MMDECLSPEC WindowInfo::WindowInfo(HWND window) :
      hwnd((HWND)0),
      parent((HWND)0),
      pid(-1),
      window_process((WNDPROC)0),
      window_rect(),
      client_rect(),
      window_title("Unknown window title"),
      window_class("Unknown class name"),
      window_type("Unknown window type")
{
   InitRects();
   SetWindowHandle(window);
}



MMDECLSPEC WindowInfo::WindowInfo(const WindowInfo& rhs) :
      hwnd(rhs.hwnd),
      parent(rhs.parent),
      pid(rhs.pid),
      window_process(rhs.window_process),
      window_rect(),
      client_rect(),
      window_title(rhs.window_title),
      window_class(rhs.window_class),
      window_type(rhs.window_type)
{
   window_rect = rhs.window_rect;
   client_rect = rhs.client_rect;
}



MMDECLSPEC void WindowInfo::InitRects() {
   window_rect.left = -1;
   window_rect.right = -1;
   window_rect.top = -1;
   window_rect.bottom = -1;
   client_rect.left = -1;
   client_rect.right = -1;
   client_rect.top = -1;
   client_rect.bottom = -1;
}  



MMDECLSPEC void WindowInfo::RefreshRects() {
   GetWindowRect(hwnd , &window_rect);
   GetClientRect(hwnd , &client_rect);
}



MMDECLSPEC void WindowInfo::RefreshWindowTitle() {
   /// TODO : remove next line
   return;
   window_title = "";
   if (hwnd) {
      const unsigned int BUFSIZE = 512;
      TCHAR buf[BUFSIZE];
      memset(buf , 0 , sizeof(TCHAR)*BUFSIZE);
      LRESULT copy_count = SendMessage(hwnd , WM_GETTEXT , (WPARAM)BUFSIZE , (LPARAM)buf);
      (void)copy_count;
      String s = buf;
      window_title = GetStlString(s);
   }
}



MMDECLSPEC void WindowInfo::RefreshWindowClassName() {
   window_class = "";
   if (hwnd) {
      const int BUFSIZE = 512;
      TCHAR buf[BUFSIZE];
      memset(buf , 0 , sizeof(TCHAR)*BUFSIZE);
      int copy_count = GetClassName(hwnd , buf , BUFSIZE);
      (void)copy_count;
      String s = buf;
      window_class = GetStlString(s);
   }
}



MMDECLSPEC void WindowInfo::RefreshInfo() {
   if (hwnd) {
      (void)GetWindowThreadProcessId(hwnd , &pid);
      window_process = (WNDPROC)GetWindowLong(hwnd , GWL_WNDPROC);
      RefreshWindowTitle();
      RefreshWindowClassName();
      RefreshRects();
   }
}



MMDECLSPEC void WindowInfo::SetWindowHandle(HWND window) {
   hwnd = window;
   parent = GetParent(hwnd);
   RefreshInfo();
}



MMDECLSPEC void WindowInfo::SetWindowTypeString(string type) {
   window_type = type;
}



MMDECLSPEC string WindowInfo::GetWindowInfoString() {
   int l = window_rect.left;
   int t = window_rect.top;
   int r = window_rect.right;
   int b = window_rect.bottom;
   int w = r - l;
   int h = b - t;
   return StringPrintF("%-20s : HWND = %p , PARENT = %p , PID = %i\nPOS = %i,%i,%i,%i , DIM = %i x %i\nTitle = '%s' , Class ='%s'\n",
                       window_type.c_str() , hwnd , parent , pid , l,t,r,b , w,h , window_title.c_str() , window_class.c_str());
}



MMDECLSPEC void WindowInfo::RefreshAreas() {
   RefreshRects();
}



MMDECLSPEC bool WindowInfo::WindowContains(int xpos , int ypos) {
   return RectContains(window_rect , xpos , ypos);
}



MMDECLSPEC bool WindowInfo::ClientContains(int xpos , int ypos) {
   return RectContains(client_rect , xpos , ypos);
}




