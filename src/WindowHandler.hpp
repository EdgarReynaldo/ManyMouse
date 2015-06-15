


#ifndef WindowHandler_HPP
#define WindowHandler_HPP




#include "AllegroAndWindows.hpp"
///#include "Mouse.hpp"
class MouseController;

#include "String.hpp"
#include "VisualLogger.hpp"
#include "WindowInfo.hpp"


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



class WindowHandler {
   
   map<HWND , WindowInfo*> window_info_map;
   deque<HWND> hwnds_zorder;// front is front - ie on top of everything else

   HWND desktop_window;
   HWND shell_window;
   HWND taskbar_window;
   
   HWND program_window;
   HWND test_window;
   
   MouseController* mc;
   
   vector<HWND> mice_windows;





public :

//   WindowHandler(MouseController* mouse_controller);
   WindowHandler(MouseController* mouse_controller) :
         window_info_map(),
         hwnds_zorder(),
         desktop_window(0),
         shell_window(0),
         taskbar_window(0),
         program_window(0),
         test_window(0),
         mc(mouse_controller),
         mice_windows()
   {
      assert(mouse_controller);
   }

   ~WindowHandler();

   void GetMiceWindows();
   void EnumerateWindows();

   void ClearWindowInfo();
   void RefreshWindowInfo();
   void SetOurWindows(HWND program_window_handle , HWND test_window_handle);
   void PrintWindowInfo();

   bool IsMouseWindow(HWND hwnd);

   HWND GetWindowAtPos(int xpos , int ypos);

   WindowInfo GetWindowInfoFromHandle(HWND hwnd);


   /// btn field : LMB = 1 , MMB = 2 , RMB = 3
//   void HandleButton(int btn , bool down , int bx , int by);
   void HandleButton(int btn , bool down , int bx , int by) {
      if (down && btn != 0) {
         HWND hwnd = GetWindowAtPos(bx , by);
         /*      
         typedef struct {
           UINT  cbSize;
           HWND  hwnd;
           DWORD dwFlags;
           UINT  uCount;
           DWORD dwTimeout;
         } FLASHWINFO, *PFLASHWINFO;
         */
         FLASHWINFO flash_info;
         flash_info.cbSize = sizeof(FLASHWINFO);
         flash_info.hwnd = hwnd;
         flash_info.dwFlags = FLASHW_ALL;
         flash_info.uCount = 3;
         flash_info.dwTimeout = 500;
         
         FlashWindowEx(&flash_info);
      }
   }

/*
   void HandleLeftButtonDown(int cx , int cy);
   void HandleLeftButtonDown(int cx , int cy) {
      
   }
   void HandleMiddleButtonDown(int cx , int cy);
   void HandleMiddleButtonDown(int cx , int cy) {
      
   }
   void HandleRightButtonDown(int cx , int cy);
   void HandleRightButtonDown(int cx , int cy) {
      
   }
   void HandleLeftButtonUp(int cx , int cy);
   void HandleLeftButtonUp(int cx , int cy) {
      
   }
   void HandleMiddleButtonUp(int cx , int cy);
   void HandleMiddleButtonUp(int cx , int cy) {
      
   }
   void HandleRightButtonUp(int cx , int cy);
   void HandleRightButtonUp(int cx , int cy) {
      
   }
*/



/**


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
//*/

   
};



/*
LRESULT WINAPI SendMessage(
  _In_ HWND   hWnd,
  _In_ UINT   Msg,
  _In_ WPARAM wParam,
  _In_ LPARAM lParam
);
*/




#endif // WindowHandler_HPP

