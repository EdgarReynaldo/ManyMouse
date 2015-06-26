


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

   HWND hwnd;
   HWND oldhwnd;
   LRESULT htmsg;
   LRESULT oldhtmsg;



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
         mice_windows(),
         hwnd(0),
         oldhwnd(0),
         htmsg(0),
         oldhtmsg(0)
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

   bool IsMouseWindow(HWND hwndA);

   HWND GetWindowAtPos(int xpos , int ypos);

   WindowInfo GetWindowInfoFromHandle(HWND hwndA);


//   void HandleMouseMove(Mouse* m);//int nmx , int nmy
   void HandleMouseMove(Mouse* m) {
      if (!m) {
         log.Log("WindowHandler::HandleMouseMove - Mouse* m is NULL\n");
         return;
      }
      int nmx = m->X();
      int nmy = m->Y();
      hwnd = GetWindowAtPos(nmx , nmy);
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
      oldhwnd = hwnd;
      hwnd = GetWindowAtPos(bx , by);
      if (!hwnd) {
         // No window under the button press location
         return;
      }
      if (btn >= 1 && btn <= 3) {
         if (down) {
/*
LRESULT WINAPI SendMessage(
  _In_ HWND   hWnd,
  _In_ UINT   Msg,
  _In_ WPARAM wParam,
  _In_ LPARAM lParam
);
*/
            oldhtmsg = htmsg;

            {
               WPARAM wp = 0;
               LPARAM lp = MAKELPARAM((short)bx , (short)by);
               htmsg = SendMessage(hwnd , WM_NCHITTEST , wp , lp);
               if ((htmsg != oldhtmsg) || (hwnd != oldhwnd)) {
                  log.Log("htmsg = %d\n" , htmsg);
               }
            }
            {
               WPARAM wp = MAKEWPARAM(WA_ACTIVE , 0);
               LPARAM lp = 0;
               
               PostMessage(hwnd , WM_ACTIVATE , wp , lp);
            }
         }
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
/*
WM_NCHITTEST return value

Return code/value	Description

HTBORDER
18

In the border of a window that does not have a sizing border.

HTBOTTOM
15

In the lower-horizontal border of a resizable window (the user can click the mouse to resize the window vertically).

HTBOTTOMLEFT
16

In the lower-left corner of a border of a resizable window (the user can click the mouse to resize the window diagonally).

HTBOTTOMRIGHT
17

In the lower-right corner of a border of a resizable window (the user can click the mouse to resize the window diagonally).

HTCAPTION
2

In a title bar.

HTCLIENT
1

In a client area.

HTCLOSE
20

In a Close button.

HTERROR
-2

On the screen background or on a dividing line between windows (same as HTNOWHERE, except that the DefWindowProc function produces a system beep to indicate an error).

HTGROWBOX
4

In a size box (same as HTSIZE).

HTHELP
21

In a Help button.

HTHSCROLL
6

In a horizontal scroll bar.

HTLEFT
10

In the left border of a resizable window (the user can click the mouse to resize the window horizontally).

HTMENU
5

In a menu.

HTMAXBUTTON
9

In a Maximize button.

HTMINBUTTON
8

In a Minimize button.

HTNOWHERE
0

On the screen background or on a dividing line between windows.

HTREDUCE
8

In a Minimize button.

HTRIGHT
11

In the right border of a resizable window (the user can click the mouse to resize the window horizontally).

HTSIZE
4

In a size box (same as HTGROWBOX).

HTSYSMENU
3

In a window menu or in a Close button in a child window.

HTTOP
12

In the upper-horizontal border of a window.

HTTOPLEFT
13

In the upper-left corner of a window border.

HTTOPRIGHT
14

In the upper-right corner of a window border.

HTTRANSPARENT
-1

In a window currently covered by another window in the same thread (the message will be sent to underlying windows in the same thread until one of them returns a code that is not HTTRANSPARENT).

HTVSCROLL
7

In the vertical scroll bar.

HTZOOM
9 

In a Maximize button.

*/




#endif // WindowHandler_HPP

