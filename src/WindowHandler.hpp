


#ifndef WindowHandler_HPP
#define WindowHandler_HPP




#include "AllegroAndWindows.hpp"

#include "Mouse.hpp"

#include "VisualLogger.hpp"

#include <vector>
using std::vector;

#include <deque>
using std::deque;



/*
   The window handler class is responsible for keeping track of all the desktop windows through their
   HWND handles, for routing mouse input to those windows, and to find their z-order so we can send
   mouse messages to the correct window.
*/



BOOL CALLBACK EnumerationProcess(HWND hwindow , LPARAM lp);

class WindowInfo {

public :
   HWND hwnd;
   DWORD pid;
   WNDPROC window_process;

   string window_title;
   string class_name;

//   WindowInfo();
   WindowInfo() :
         hwnd(0),
         pid(-1),
         window_process((WNDPROC)0),
         window_title("Unknown window title"),
         class_name("Unknown class name")
   {
      
   }


/*
   void SetWindowHandle(HWND window);
   void SetWindowHandle(HWND window) {
      hwnd = window;
      RefreshInfo();
   }
   void RefreshWindowTitle();
   void RefreshWindowTitle() {
      window_title = "";
      if (hwnd) {
         const unsigned int BUFSIZE = 512;
         TCHAR buf[BUFSIZE];
         memset(buf , 0 , sizeof(TCHAR)*BUFSIZE);
         SendMessage(hwnd , WM_GETTEXT , (WPARAM)BUFSIZE , (LPARAM)buf);
         
      }
   }
   
   void RefreshClassName();
   void RefreshClassName() {
      
   }
   
   void RefreshInfo();
   void RefreshInfo() {
      if (hwnd) {
         (void)GetWindowThreadProcessId(hwnd , &pid);
         window_process = GetWindowLong(hwnd , GWL_WNDPROC);
         RefreshWindowTitle();
         RefreshClassName();
      }
   }
*/   
};

/*
LRESULT WINAPI SendMessage(
  _In_ HWND   hWnd,
  _In_ UINT   Msg,
  _In_ WPARAM wParam,
  _In_ LPARAM lParam
);
*/

class WindowHandler {
   
   vector<HWND> desktop_windows;
   vector<HWND> mice_windows;
   vector<HWND> other_windows;
   HWND program_window;
   HWND desktop_window;
   
   HWND taskbar_window;
   HWND shell_window;
   
   deque<WindowInfo> all_window_info;// All current windows, sorted in z-order, first on bottom, last on top
   
   MouseController* mc;
   
public :
   WindowHandler(MouseController* mscontroller);
   
   void SetController(MouseController* mscontroller);
   void EnumerateDesktopWindows();
   void GetMiceWindows();
   void Refresh();
   void SetProgramWindow(HWND main);
   
   bool IsProgramWindow(HWND hwindow);
   bool IsTheDesktopWindow(HWND hwindow);
   bool IsMouseWindow(HWND hwindow);
   bool IsOurWindow(HWND hwindow);
   
   void PrintWindows();

//   void PrintAllWindows();
   void PrintAllWindows() {
      deque<WindowInfo>::iterator it = all_window_info.begin();
      while (it != all_window_info.end()) {
         log.Log("HWND : %p , PID : %lu\n" , it->hwnd , it->pid);
         ++it;
      }
   }

//   void FindAllWindowsZOrder();
   void FindAllWindowsZOrder() {
      all_window_info.clear();
      
      WindowInfo wi;
      DWORD unused;
      
      HWND top = GetTopWindow(NULL);
      
      wi.hwnd = top;
      unused = GetWindowThreadProcessId(top , &wi.pid);
      
      (void)unused;
      
      if (IsWindowVisible(top) && !IsIconic(top)) {
         all_window_info.push_back(wi);
      }
      
      HWND next = top;
      
      while ((next = GetNextWindow(next , GW_HWNDNEXT))) {
         wi.hwnd = next;
         unused = GetWindowThreadProcessId(next , &wi.pid);
         if (IsWindowVisible(next) && !IsIconic(next)) {
            all_window_info.push_front(wi);
         }
      }
      
      log.Log("Find all windows found %u windows\n" , all_window_info.size());
      
   }


   
};




#endif // WindowHandler_HPP

