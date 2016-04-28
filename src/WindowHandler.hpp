


#ifndef WindowHandler_HPP
#define WindowHandler_HPP




#include "AllegroAndWindows.hpp"
#include "Mouse.hpp"
class MouseController;

///#include "String.hpp"
#include "VisualLogger.hpp"
#include "WindowInfo.hpp"
#include "WindowTree.hpp"



using namespace ManyMouse;

#include <cstdio>

#include <vector>
using std::vector;

#include <deque>
using std::deque;



/*
   The window handler class is responsible for keeping track of all the desktop windows through their
   HWND handles, for routing mouse input to those windows, and to find their z-order so we can send
   mouse messages to the correct window.
*/



const char* HTCODE_to_str(int htcode);

bool GetDesktopBounds(RECT* b);


BOOL CALLBACK EnumerationProcess(HWND hwindow , LPARAM lp);

HWND FindTopMostChild(HWND parent , POINT abs_coords);



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

   bool system_mouse_on;

   deque<HWND> all_windows;
   deque<HWND> all_visible_windows;
   deque<HWND> all_other_windows;// all windows except our own
   deque<HWND> disabled_windows;
//   bool windows_disabled;

   HWND active_window;

   Mutex mutex;

   WindowTree window_tree;

   WindowNode base_node;
   WindowNode child_node;


   friend BOOL CALLBACK EnumerateWindowsProcess(HWND hwindow , LPARAM lp);



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
         oldhtmsg(0),
         system_mouse_on(true),
         all_windows(),
         all_other_windows(),
         disabled_windows(),
         active_window(GetForegroundWindow()),
         mutex(),
         window_tree(mouse_controller),
         base_node(),
         child_node()
   {
      assert(mouse_controller);
      if (!mutex.Init()) {
         ManyMouse::log.Log("WindowHandler::WindowHandler - Failed to init mutex.\n");
      }
   }

   ~WindowHandler();

   bool GetBaseNode(POINT p , WindowNode& store_node);
   bool GetChildNode(POINT p , WindowNode& store_node);

///   void HandleShellHookInfo(int code , WPARAM wp , LPARAM lp);
   void HandleShellHookInfo(int code , WPARAM wp , LPARAM lp) {

//   int codes[4] = {HSHELL_WINDOWACTIVATED , HSHELL_WINDOWCREATED
      /// TODO : This function *should* be tracking window changes but for some reason it's not doing it fully/right
      if ((code == HSHELL_WINDOWACTIVATED) ||
         (code == HSHELL_WINDOWCREATED) ||
         (code == HSHELL_WINDOWDESTROYED) ||
         (code == HSHELL_GETMINRECT)) {

         mutex.Lock();
         // Handle our hook information
         ManyMouse::log.Log("HandleShellHookInfo called\n");

         EnumerateWindows();

         mutex.Unlock();
      }
   }


   void PrintAllWindows();


   void GetMiceWindows();
   void EnumerateWindows();

   void ClearWindowInfo();
   void RefreshWindowInfo();
   void SetOurWindows(HWND program_window_handle , HWND log_window_handle , HWND test_window_handle);
   void PrintWindowInfo();

   bool NotOurWindow(HWND hwindow);
   bool IsMouseWindow(HWND hwndA);

   HWND GetWindowAtPos(int xpos , int ypos);


   WindowInfo GetWindowInfoFromHandle(HWND hwndA);

   void ToggleSystemMouseOnOff(bool on);
   bool SystemMouseOn() {return system_mouse_on;}





//   void HandleMouseMove(Mouse* m);//int nmx , int nmy
   void HandleMouseMove(Mouse* m) {
      if (!m) {
         ManyMouse::log.Log("WindowHandler::HandleMouseMove - Mouse* m is NULL\n");
         return;
      }
      if (!m->MouseMoved()) {
         return;
      }
      const int nmx = m->X();
      const int nmy = m->Y();
      POINT p;
      p.x = nmx;
      p.y = nmy;

///      hwnd = GetWindowAtPos(nmx , nmy);
      WindowNode base_node;
      WindowNode child_node;
   
      if (!window_tree.GetBaseWindowNode(p , base_node)) {
         return;
      }

      hwnd = base_node.hwindow;

      if (!window_tree.GetTopChildWindowNode(p , child_node)) {
          return;
      }

      hwnd = child_node.hwindow;

      if (hwnd) {

// EnableWindow is dangerous - if you disable a window and never re-enable it it can't get input anymore
///         EnableWindow(hwnd , system_mouse_on);
 
         bool nc = false;/// indicates non-client area
         {
            WindowInfo info = GetWindowInfoFromHandle(hwnd);
            WPARAM wp = 0;
            LPARAM lp = MAKELPARAM((short)nmx , (short)nmy);
            htmsg = SendMessage(hwnd , WM_NCHITTEST , wp , lp);
            if (htmsg == HTCLOSE ||
                htmsg == HTSIZE ||
                htmsg == HTMAXBUTTON ||
                htmsg == HTMINBUTTON ||
                htmsg == HTCAPTION) {
               nc = true;
            }
///            log.Log("htmsg = %d\n" , htmsg);
            if ((htmsg != oldhtmsg) || (hwnd != oldhwnd)) {
               ManyMouse::log.Log("htmsg = %s (%d) , window title = '%s'\n" , HTCODE_to_str(htmsg) , htmsg , info.WindowTitle().c_str());
            }
         }

///         assert(ScreenToClient(hwnd , &p));
///         nmx = p.x;
///         nmy = p.y;

//         RECT clrect;
//         GetClientRect(hwnd , &clrect);
//         nmx -= clrect.left;
//         nmy -= clrect.top;


         ScreenToClient(hwnd , &p);

         WPARAM wp = 0;// TODO : Encode modifier flags, see WM_LBUTTONDOWN
         LPARAM lp = MAKELPARAM((short)p.x , (short)p.y);

         if (nc) {
            PostMessage(hwnd , WM_NCMOUSEMOVE , wp , lp);
            PostMessage(hwnd , WM_NCMOUSEHOVER , wp , lp);
            PostMessage(hwnd , WM_MOUSEMOVE , wp , lp);
            PostMessage(hwnd , WM_MOUSEHOVER , wp , lp);
         }
         else {
            PostMessage(hwnd , WM_MOUSEMOVE , wp , lp);
            PostMessage(hwnd , WM_MOUSEHOVER , wp , lp);
         }
      }

      oldhwnd = hwnd;
      oldhtmsg = htmsg;

   }

   /// btn field : LMB = 1 , MMB = 2 , RMB = 3
//   void HandleButton(int btn , bool down , int bx , int by);
   void HandleButton(Mouse* m , int btn , bool down , int bx , int by) {
      if (!m) {
         return;
      }

      
      POINT p;
      p.x = bx;
      p.y = by;

      if (!GetBaseNode(p , base_node)) {
         return;
      }
      if (!GetChildNode(p , child_node)) {
         return;
      }

      oldhwnd = hwnd;

      hwnd = base_node.hwindow;
///      hwnd = GetWindowAtPos(bx , by);

      if (!hwnd) {
         // No window under the button press location
         return;
      }

///      HWND childhwnd = ChildWindowFromPoint(hwnd , p);
///      HWND childhwnd = ChildWindowFromPointEx(hwnd , p , CWP_SKIPINVISIBLE | CWP_SKIPTRANSPARENT);

///      HWND childhwnd = FindTopMostChild(hwnd , p);
      HWND childhwnd = child_node.hwindow;

      WindowInfo winfo;
      WindowInfo winfo2;
      winfo.SetWindowHandle(hwnd);
      winfo2.SetWindowHandle(childhwnd);

      ManyMouse::log.Log("\n");

      ManyMouse::log.Log(winfo.GetWindowInfoString());
      ManyMouse::log.Log(winfo2.GetWindowInfoString());

      unsigned int msg[16] = {
         0 , WM_LBUTTONDOWN , WM_MBUTTONDOWN , WM_RBUTTONDOWN , 0 , WM_LBUTTONUP , WM_MBUTTONUP , WM_RBUTTONUP ,
         0 , WM_NCLBUTTONDOWN , WM_NCMBUTTONDOWN , WM_NCRBUTTONDOWN , 0 , WM_NCLBUTTONUP , WM_NCMBUTTONUP , WM_NCRBUTTONUP
      };
      const char* strs[16] = {
         0 , "WM_LBUTTONDOWN" , "WM_MBUTTONDOWN" , "WM_RBUTTONDOWN" , 0 , "WM_LBUTTONUP" , "WM_MBUTTONUP" , "WM_RBUTTONUP" ,
         0 , "WM_NCLBUTTONDOWN" , "WM_NCMBUTTONDOWN" , "WM_NCRBUTTONDOWN" , 0 , "WM_NCLBUTTONUP" , "WM_NCMBUTTONUP" , "WM_NCRBUTTONUP"
      };

      hwnd = childhwnd;

      bool nc = false;
      if (btn >= 1 && btn <= 3) {
         if (down) {
            WPARAM wp = 0;
            LPARAM lp = MAKELPARAM((short)bx , (short)by);
            if (hwnd) {

               if (hwnd != active_window) {
                  SetForegroundWindow(hwnd);

                  ALLEGRO_ASSERT(m);

                  BringWindowToTop(m->GetMouseWindowHandle());
               }
               active_window = GetForegroundWindow();

               if (active_window == m->GetMouseWindowHandle()) {
                  printf("Active window is mouse.\n");
               }

               htmsg = SendMessage(hwnd , WM_NCHITTEST , wp , lp);
               if (htmsg == HTCLOSE ||
                   htmsg == HTSIZE ||
                   htmsg == HTMAXBUTTON ||
                   htmsg == HTMINBUTTON ||
                   htmsg == HTCAPTION) {
                  nc = true;
               }

               int index = (nc?8:0) + (down?0:4) + btn;
               const char* str = strs[index];
               if (str) {
                  ManyMouse::log.Log("%s sent to hwnd %p\n" , str , hwnd);
               }

               switch (htmsg) {
               case HTCLOSE :
                  wp = SC_CLOSE;
                  DestroyWindow(hwnd);
                  SendMessage(hwnd , WM_SYSCOMMAND , wp , lp);
                  break;
               case HTMAXBUTTON :
                  wp = SC_MAXIMIZE;
                  ShowWindow(hwnd , SW_MAXIMIZE);
                  SendMessage(hwnd , WM_SYSCOMMAND , wp , lp);
                  break;
               case HTMINBUTTON :
                  wp = SC_MINIMIZE;
                  ShowWindow(hwnd , SW_MINIMIZE);
                  SendMessage(hwnd , WM_SYSCOMMAND , wp , lp);
                  break;
               case HTSIZE :
                  wp = SC_SIZE;
                  ShowWindow(hwnd , SW_RESTORE);
                  SendMessage(hwnd , WM_SYSCOMMAND , wp , lp);
                  break;
//               case HTREDUCE :/// Same as HTMINBUTTON
//                  wp = SC_RESTORE;
//                  SendMessage(hwnd , WM_SYSCOMMAND , wp , lp);
//                  break;
               case HTCAPTION :
                  wp = SC_MOVE;
                  SendMessage(hwnd , WM_SYSCOMMAND , wp , lp);
                  break;
               default :
                  break;
               }

/*
               /// TODO : Need MouseActivate and NCActivate or something
               wp = MAKEWPARAM(WA_ACTIVE , 0);
               lp = 0;

               PostMessage(hwnd , WM_ACTIVATE , wp , lp);

               wp = (WPARAM)GetParent(hwnd);
               lp = MAKELPARAM(htmsg , msg[nc?8:0 + (down?0:4) + btn]);

               PostMessage(hwnd , WM_MOUSEACTIVATE , wp , lp);
*/
            }
         }

         /// Send button message
         p.x = bx;
         p.y = by;
         if (!ScreenToClient(hwnd , &p)) {
            ManyMouse::log.Log("WindowHandler::HandleButton() - ScreenToClient failed\n");
            ManyMouse::log.Log("GetLastError reports error %i\n" , GetLastError());
         }
         else {
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

            SendMessage(hwnd , msg[nc?8:0 + (down?0:4) + btn] , wp , lp);
///            PostMessage(hwnd , msg[nc?8:0 + (down?0:4) + btn] , wp , lp);
            
            mc->BringMiceToFront();
            
            EnumerateWindows();
///            window_tree.EnumerateTree();
         }
      }
   }

};
/*
WM_NCHITTEST return value

Return code/value	Description

HTERROR -2
On the screen background or on a dividing line between windows (same as HTNOWHERE, except that the DefWindowProc function produces a system beep to indicate an error).

HTTRANSPARENT -1
In a window currently covered by another window in the same thread (the message will be sent to underlying windows in the same thread until one of them returns a code that is not HTTRANSPARENT).

HTNOWHERE 0
On the screen background or on a dividing line between windows.

HTCLIENT 1
In a client area.

HTCAPTION 2
In a title bar.

HTSYSMENU 3
In a window menu or in a Close button in a child window.

HTSIZE 4
In a size box (same as HTGROWBOX).

HTGROWBOX 4
In a size box (same as HTSIZE).

HTMENU 5
In a menu.

HTHSCROLL 6
In a horizontal scroll bar.

HTVSCROLL 7
In the vertical scroll bar.

HTREDUCE 8
In a Minimize button.

HTMINBUTTON 8
In a Minimize button.

HTMAXBUTTON 9
In a Maximize button.

HTZOOM 9
In a Maximize button.

HTLEFT 10
In the left border of a resizable window (the user can click the mouse to resize the window horizontally).


HTRIGHT 11
In the right border of a resizable window (the user can click the mouse to resize the window horizontally).

HTTOP 12
In the upper-horizontal border of a window.

HTTOPLEFT 13
In the upper-left corner of a window border.

HTTOPRIGHT 14
In the upper-right corner of a window border.

HTBOTTOM 15
In the lower-horizontal border of a resizable window (the user can click the mouse to resize the window vertically).

HTBOTTOMLEFT 16
In the lower-left corner of a border of a resizable window (the user can click the mouse to resize the window diagonally).

HTBOTTOMRIGHT 17
In the lower-right corner of a border of a resizable window (the user can click the mouse to resize the window diagonally).

HTBORDER 18
In the border of a window that does not have a sizing border.

HTCLOSE 20
In a Close button.

HTHELP 21
In a Help button.

*/

/*
Parameters

wParam

The type of system command requested. This parameter can be one of the following values.
Value	Meaning

SC_CLOSE 0xF060
Closes the window.

SC_CONTEXTHELP 0xF180
Changes the cursor to a question mark with a pointer. If the user then clicks a control in the dialog box, the control receives a WM_HELP message.

SC_DEFAULT 0xF160
Selects the default item; the user double-clicked the window menu.

SC_HOTKEY 0xF150
Activates the window associated with the application-specified hot key. The lParam parameter identifies the window to activate.

SC_HSCROLL 0xF080
Scrolls horizontally.

SCF_ISSECURE 0x00000001
Indicates whether the screen saver is secure.

SC_KEYMENU 0xF100
Retrieves the window menu as a result of a keystroke. For more information, see the Remarks section.

SC_MAXIMIZE 0xF030
Maximizes the window.

SC_MINIMIZE 0xF020
Minimizes the window.

SC_MONITORPOWER 0xF170
Sets the state of the display. This command supports devices that have power-saving features, such as a battery-powered personal computer.

The lParam parameter can have the following values:

  -1 (the display is powering on)
  1 (the display is going to low power)
  2 (the display is being shut off)

SC_MOUSEMENU 0xF090
Retrieves the window menu as a result of a mouse click.

SC_MOVE 0xF010
Moves the window.

SC_NEXTWINDOW 0xF040
Moves to the next window.

SC_PREVWINDOW 0xF050
Moves to the previous window.

SC_RESTORE 0xF120
Restores the window to its normal position and size.

SC_SCREENSAVE 0xF140
Executes the screen saver application specified in the [boot] section of the System.ini file.

SC_SIZE 0xF000
Sizes the window.

SC_TASKLIST 0xF130
Activates the Start menu.

SC_VSCROLL 0xF070
Scrolls vertically.
 
 //*/


#endif // WindowHandler_HPP

