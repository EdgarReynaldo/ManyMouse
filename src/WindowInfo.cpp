

#include "WindowInfo.hpp"

#include "String.hpp"



bool RectContains(RECT r , int xpos , int ypos) {
   return ((xpos >= r.left) && (ypos >= r.top) && (xpos < r.right) && (ypos < r.bottom));
}



string RectString(RECT rect) {
   int l = rect.left;
   int r = rect.right;
   int t = rect.top;
   int b = rect.bottom;
   int w = r - l;
   int h = b - t;
   return StringPrintF("%d,%d : %d x %d" , l , r , w , h);
}



void WindowInfo::InitRects() {
   window_rect.left = -1;
   window_rect.right = -1;
   window_rect.top = -1;
   window_rect.bottom = -1;
   client_rect.left = -1;
   client_rect.right = -1;
   client_rect.top = -1;
   client_rect.bottom = -1;
}  



void WindowInfo::RefreshRects() {
   GetWindowRect(hwnd , &window_rect);
   GetClientRect(hwnd , &client_rect);
}



void WindowInfo::RefreshWindowTitle() {
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



void WindowInfo::RefreshWindowClassName() {
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



void WindowInfo::RefreshInfo() {
   if (hwnd) {
      (void)GetWindowThreadProcessId(hwnd , &pid);
      window_process = (WNDPROC)GetWindowLong(hwnd , GWL_WNDPROC);
      RefreshWindowTitle();
      RefreshWindowClassName();
      RefreshRects();
   }
}



void WindowInfo::SetWindowHandle(HWND window) {
   hwnd = window;
   parent = GetParent(hwnd);
   RefreshInfo();
}



void WindowInfo::SetWindowTypeString(string type) {
   window_type = type;
}



string WindowInfo::GetWindowInfoString() {
   int l = window_rect.left;
   int t = window_rect.top;
   int r = window_rect.right;
   int b = window_rect.bottom;
   int w = r - l;
   int h = b - t;
   return StringPrintF("%-20s : HWND = %p , PARENT = %p , PID = %i\nPOS = %i,%i,%i,%i , DIM = %i x %i\nTitle = '%s' , Class ='%s'\n",
                       window_type.c_str() , hwnd , parent , pid , l,t,r,b , w,h , window_title.c_str() , window_class.c_str());
}



void WindowInfo::RefreshAreas() {
   RefreshRects();
}



bool WindowInfo::WindowContains(int xpos , int ypos) {
   return RectContains(window_rect , xpos , ypos);
}



bool WindowInfo::ClientContains(int xpos , int ypos) {
   return RectContains(client_rect , xpos , ypos);
}




