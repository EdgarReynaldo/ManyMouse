

#include "WindowInfo.hpp"

#include "String.hpp"



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
   return StringPrintF("%-20s : HWND = %p , PID = %i\nPOS = %i,%i,%i,%i , DIM = %i x %i\nTitle = '%s' , Class = '%s'\n",
                       window_type.c_str() , hwnd , pid , l,t,r,b , w,h , window_title.c_str() , window_class.c_str());
}




