


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
   HWND log_window;
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
         log_window(0),
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
   void SetOurWindows(HWND program_window_handle , HWND log_window_handle , HWND test_window_handle);
   void PrintWindowInfo();

   bool IsMouseWindow(HWND hwnd);

   HWND GetWindowAtPos(int xpos , int ypos);

   WindowInfo GetWindowInfoFromHandle(HWND hwnd);


//   void HandleMouseMove(Mouse* m);//int nmx , int nmy
   void HandleMouseMove(Mouse* m) {
      if (!m) {
         log.Log("WindowHandler::HandleMouseMove - Mouse* m is NULL\n");
         return;
      }
      int nmx = m->X();
      int nmy = m->Y();
      HWND hwnd = GetWindowAtPos(nmx , nmy);
      if (hwnd) {
            
         POINT p;
         p.x = nmx;
         p.y = nmy;
         assert(ScreenToClient(hwnd , &p));
         nmx = p.x;
         nmy = p.y;
         
//         RECT clrect;
//         GetClientRect(hwnd , &clrect);
//         nmx -= clrect.left;
//         nmy -= clrect.top;

         WPARAM wp = 0;// TODO : Encode modifier flags, see WM_LBUTTONDOWN
///         LPARAM lp;
///         lp = (((nmy << 16)&0xffff0000) | (((short)nmx)&0xffff));
         LPARAM lp = MAKELPARAM((short)nmx , (short)nmy);
         
         
         PostMessage(hwnd , WM_MOUSEMOVE , wp , lp);
      }
   }
   
   /// btn field : LMB = 1 , MMB = 2 , RMB = 3
//   void HandleButton(int btn , bool down , int bx , int by);
   void HandleButton(int btn , bool down , int bx , int by) {
      HWND hwnd = GetWindowAtPos(bx , by);
      if (!hwnd) {
         // No window under the button press location
         return;
      }
      if (down && btn != 0) {
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
         flash_info.uCount = 8;
         flash_info.dwTimeout = 100;
         
         FlashWindowEx(&flash_info);

      }
      if (btn >= 1 && btn <= 3) {
         unsigned int msg[8] = {
            0 , WM_LBUTTONDOWN , WM_MBUTTONDOWN , WM_RBUTTONDOWN , 0 , WM_LBUTTONUP , WM_MBUTTONUP , WM_RBUTTONUP
         };
   
         POINT p;
         p.x = bx;
         p.y = by;
         assert(ScreenToClient(hwnd , &p));
         bx = p.x;
         by = p.y;

//         RECT clrect;
//         GetClientRect(hwnd , &clrect);
//         bx -= clrect.left;
//         by -= clrect.top;

         WPARAM wp = 0;// TODO : Encode modifier flags, see WM_LBUTTONDOWN
///         LPARAM lp;
///         lp = (((by << 16)&0xffff0000) | (((short)bx)&0xffff));
         LPARAM lp = MAKELPARAM((short)bx , (short)by);// this is a little clearer than and'ing and or'ing

         PostMessage(hwnd , msg[(down?0:4) + btn] , wp , lp);
      }
   }
   
};





#endif // WindowHandler_HPP

