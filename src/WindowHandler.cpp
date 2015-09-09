


#include "Mouse.hpp"

#include "WindowHandler.hpp"
#include "MouseController.hpp"
#include "VisualLogger.hpp"

#include "String.hpp"


#include <cstdio>

/*
BOOL WINAPI EnumChildWindows(
  _In_opt_ HWND        hWndParent,
  _In_     WNDENUMPROC lpEnumFunc,
  _In_     LPARAM      lParam
);
*/


bool GetDesktopBounds(RECT* b) {
   b->left = 0;
   b->right = 0;
   b->top = 0;
   b->bottom = 0;
   return GetWindowRect(GetDesktopWindow() , b);
}



///BOOL CALLBACK EnumerateChildWindowsProcess(HWND hwindow , LPARAM lp);
BOOL CALLBACK EnumerateChildWindowsProcess(HWND hwindow , LPARAM lp) {
   deque<HWND>* pdeq = (deque<HWND>*)lp;
   ALLEGRO_ASSERT(pdeq);
   pdeq->push_back(hwindow);
   return true;
}



//friend BOOL CALLBACK EnumerateWindowsProcess(HWND hwindow , LPARAM lp);
BOOL CALLBACK EnumerateWindowsProcess(HWND hwindow , LPARAM lp) {
   WindowHandler* wh = (WindowHandler*)lp;
   ALLEGRO_ASSERT(wh);
//   deque<HWND> all_windows;
//   deque<HWND> all_visible_windows;
//   deque<HWND> all_other_windows;// all windows except our own
   wh->all_windows.push_back(hwindow);
   EnumChildWindows(hwindow , EnumerateChildWindowsProcess , (LPARAM)&wh->all_windows);
   if (IsWindowVisible(hwindow)) {
      wh->all_visible_windows.push_back(hwindow);
      EnumChildWindows(hwindow , EnumerateChildWindowsProcess , (LPARAM)&wh->all_visible_windows);
   }
   if (wh->NotOurWindow(hwindow)) {
      wh->all_other_windows.push_back(hwindow);
      EnumChildWindows(hwindow , EnumerateChildWindowsProcess , (LPARAM)&wh->all_other_windows);
   }
   return true;
}



BOOL CALLBACK EnumerateVisibleWindowsProcess(HWND hwindow , LPARAM lp) {
   deque<HWND>* pwin = (deque<HWND>*)lp;
   ALLEGRO_ASSERT(pwin);
   if (IsWindowVisible(hwindow)) {
      pwin->push_back(hwindow);
   }
   return true;
}

BOOL CALLBACK EnumerateAllWindowsProcess(HWND hwindow , LPARAM lp) {
   deque<HWND>* pwin = (deque<HWND>*)lp;
   ALLEGRO_ASSERT(pwin);
   pwin->push_back(hwindow);
   return true;
}


/*
BOOL CALLBACK EnumerationProcess(HWND hwindow , LPARAM lp) {
   deque<HWND>* pwin = (deque<HWND>*)lp;
   if (IsWindowVisible(hwindow)) {
      pwin->push_back(hwindow);
   }
   return true;
}
*/
HWND FindTopMostChild(HWND parent , POINT abs_coords) {

   if (!parent) {return NULL;}

/*
   RECT clrect;
   if (GetClientRect(parent , &clrect)) {
      abs_coords.x -= clrect.left;
      abs_coords.y -= clrect.top;
   }
   else {
      log.Log("FindTopMostChild::Failed to get client rect for window %p\n" , parent);
   }
*/
   if (!ScreenToClient(parent , &abs_coords)) {
      log.Log("FindTopMostChild() - ScreenToClient failed.\n");
   }


   HWND direct_child = parent;
   do {
         parent = direct_child;
         direct_child = ChildWindowFromPoint(parent , abs_coords);
         if (direct_child == parent) {
            return parent;//break out of loop
         }
///         direct_child = RealChildWindowFromPoint(parent , abs_coords);
   } while (direct_child);

   return parent;

/*
HWND WINAPI RealChildWindowFromPoint(
  _In_ HWND  hwndParent,
  _In_ POINT ptParentClientCoords
);
*/

   return NULL;
}



void WindowHandler::PrintAllWindows() {

   window_tree.Print();

/*
   FILE* logfile = log.GetLogFile();

   if (!logfile) {printf("Failed to get log file.\n");}

   deque<HWND>::iterator itwin = all_windows.begin();
   while (itwin != all_windows.end()) {
      HWND hwindow = *itwin;
      HWND parent = hwindow;
      int depth = 0;
      while ((parent = GetParent(parent))) {
         ++depth;
      }
      depth *= 3;
      char* buf = new char[depth + 1];
      for (int i = 0 ; i < depth ; ++i) {
         buf[i] = ' ';
      }
      buf[depth] = '\0';

      WindowInfo info;
      info.SetWindowHandle(hwindow);

      string infostr = info.GetWindowInfoString();

      vector<string> lines = SplitByNewLines(infostr);

      fprintf(logfile , "\n");
      for (unsigned int i = 0 ; i < lines.size() ; ++i) {
         fprintf(logfile , "%s%s\n" , buf , lines[i].c_str());
      }

      delete buf;
      ++itwin;
   }
*/
}



void WindowHandler::GetMiceWindows() {
   if (mc) {
      mice_windows.clear();
      mc->GetMiceWindows(&mice_windows);
   }
}



void WindowHandler::EnumerateWindows() {

   window_tree.EnumerateTree();

   ClearWindowInfo();

   GetMiceWindows();

   desktop_window = GetDesktopWindow();
   shell_window = GetShellWindow();
   taskbar_window = FindWindow("Shell_TrayWnd" , NULL);

   EnumWindows(EnumerateWindowsProcess , (LPARAM)(this));
   EnumWindows(EnumerateVisibleWindowsProcess , (LPARAM)&hwnds_zorder);

   log.Log("Desktop window handle = %p\n" , desktop_window);
   log.Log("Shell   window handle = %p\n" , shell_window);
   log.Log("Taskbar window handle = %p\n" , taskbar_window);

   deque<HWND>::iterator dit = hwnds_zorder.begin();
   while (dit != hwnds_zorder.end()) {
      HWND hwndA = *dit;

      WindowInfo* pwi = new WindowInfo();

      pwi->SetWindowHandle(hwndA);
      if (hwndA == desktop_window) {
         pwi->SetWindowTypeString("The desktop window");
      }
      else if (hwndA == shell_window) {
         pwi->SetWindowTypeString("The shell window");
      }
      else if (hwndA == taskbar_window) {
         pwi->SetWindowTypeString("The taskbar window");
      }
      else if (hwndA == program_window) {
         pwi->SetWindowTypeString("Our program window");
      }
      else if (hwndA == test_window) {
         pwi->SetWindowTypeString("Our test window");
      }
      else if (hwndA == log_window) {
         pwi->SetWindowTypeString("Our log window");
      }
      else if (IsMouseWindow(hwndA)) {
         for (unsigned int i = 0 ; i < mice_windows.size() ; ++i) {
            HWND hwndmouse = mice_windows[i];
            if (hwndA == hwndmouse) {
               pwi->SetWindowTypeString(StringPrintF("Our mouse window #%u" , i));
            }
         }
      }
      else {
         pwi->SetWindowTypeString("A user window");
      }

      window_info_map[hwndA] = pwi;

      ++dit;
   }

}




WindowHandler::~WindowHandler() {
   ClearWindowInfo();
}



void WindowHandler::ClearWindowInfo() {
   map<HWND , WindowInfo*>::iterator it = window_info_map.begin();
   while (it != window_info_map.end()) {
      WindowInfo* pwi = it->second;
      delete pwi;
      ++it;
   }
   window_info_map.clear();
   hwnds_zorder.clear();
   desktop_window = 0;
   shell_window = 0;
   taskbar_window = 0;
//      program_window = 0;
//      test_window = 0;
}



void WindowHandler::RefreshWindowInfo() {
   ClearWindowInfo();
   EnumerateWindows();
   SetOurWindows(program_window , log_window , test_window);
}



void WindowHandler::SetOurWindows(HWND program_window_handle , HWND log_window_handle , HWND test_window_handle) {
   program_window = program_window_handle;
   log_window = log_window_handle;
   test_window = test_window_handle;

   map<HWND , WindowInfo*>::iterator it;

   if (program_window) {
      it = window_info_map.find(program_window);
      if (it != window_info_map.end()) {
         WindowInfo* pwi = it->second;
         pwi->SetWindowTypeString("Our program window");
      }
   }
   if (log_window) {
      it = window_info_map.find(log_window);
      if (it != window_info_map.end()) {
         WindowInfo* pwi = it->second;
         pwi->SetWindowTypeString("Our log window");
      }
   }
   if (test_window) {
      it = window_info_map.find(test_window);
      if (it != window_info_map.end()) {
         WindowInfo* pwi = it->second;
         pwi->SetWindowTypeString("The test window");
      }
   }

   window_tree.SetOurWindow(program_window_handle);
}



void WindowHandler::PrintWindowInfo() {

   deque<HWND>::iterator itwin = hwnds_zorder.begin();
   while (itwin != hwnds_zorder.end()) {

      HWND hwndA = *itwin;

      map<HWND , WindowInfo*>::iterator itinfo = window_info_map.find(hwndA);
      if (itinfo != window_info_map.end()) {
         WindowInfo* pwi = itinfo->second;
         log.Log(pwi->GetWindowInfoString());
         log.Log("\n");
      }

      ++itwin;
   }
}



bool WindowHandler::NotOurWindow(HWND hwindow) {
   if ((hwindow != desktop_window) &&
       (hwindow != shell_window) &&
       (hwindow != taskbar_window) &&
       (hwindow != program_window) &&
       (hwindow != test_window) &&
       (hwindow != log_window) &&
       !(IsMouseWindow(hwindow)) ) {
      return true;
   }
   return false;
/*
      if (hwndA == desktop_window) {
         pwi->SetWindowTypeString("The desktop window");
      }
      else if (hwndA == shell_window) {
         pwi->SetWindowTypeString("The shell window");
      }
      else if (hwndA == taskbar_window) {
         pwi->SetWindowTypeString("The taskbar window");
      }
      else if (hwndA == program_window) {
         pwi->SetWindowTypeString("Our program window");
      }
      else if (hwndA == test_window) {
         pwi->SetWindowTypeString("Our test window");
      }
      else if (hwndA == log_window) {
         pwi->SetWindowTypeString("Our log window");
      }
      else if (IsMouseWindow(hwndA)) {
         for (unsigned int i = 0 ; i < mice_windows.size() ; ++i) {
            HWND hwndmouse = mice_windows[i];
            if (hwndA == hwndmouse) {
               pwi->SetWindowTypeString(StringPrintF("Our mouse window #%u" , i));
            }
         }
      }

*/}



bool WindowHandler::IsMouseWindow(HWND hwndA) {
   for (unsigned int i = 0 ; i < mice_windows.size() ; ++i) {
      HWND hwndmouse = mice_windows[i];
      if (hwndA == hwndmouse) {
         return true;
      }
   }
   return false;
}



HWND WindowHandler::GetWindowAtPos(int xpos , int ypos) {

   deque<HWND>::iterator it = hwnds_zorder.begin();
   while (it != hwnds_zorder.end()) {

      HWND hwndA = *it;

      /// Need to ignore mouse windows here
      if (IsMouseWindow(hwndA)) {
         ++it;
         continue;
      }

      if (hwndA == program_window) {
         ++it;
         continue;
      }


      RECT r;
      if (!GetWindowRect(hwndA , &r)) {
         log.Log("Failed to get window rect for hwnd %p\n" , hwndA);
      }
      else if (RectContains(r , xpos , ypos)) {
         return hwndA;
      }
//      WindowInfo* pwi =

      ++it;
   }

   return (HWND)0;
}




WindowInfo WindowHandler::GetWindowInfoFromHandle(HWND hwndA) {

   WindowInfo winfo;
   winfo.SetWindowHandle(hwndA);
   return winfo;

//   if (window_info_map.find(hwnd) != window_info_map.end()) {
//      return *(window_info_map[hwndA]);
//   }
   return WindowInfo();
}



void WindowHandler::ToggleSystemMouseOnOff(bool on) {
   system_mouse_on = on;
}



/*

WindowHandler::WindowHandler(MouseController* mscontroller) :
      desktop_windows(),
      mice_windows(),
      other_windows(),
      program_window(0),
      desktop_window(0),
      all_window_info(),
      mc(0)
{
   SetController(mscontroller);
}



void WindowHandler::SetController(MouseController* mscontroller) {
   mc = mscontroller;
}



void WindowHandler::EnumerateDesktopWindows() {
   FreeWindowInfo();
   hwnds_zorder.clear();
   EnumWindows(EnumerationProcess , (LPARAM)(&hwnds_zorder));
   desktop_window = GetDesktopWindow();
   shell_window = GetShellWindow();
   taskbar_window = FindWindow("Shell_TrayWnd" , NULL);
}





// if( (IsWindowVisible(hwnd) || IsIconic(hwnd)) &&
//       (GetWindowLong(hwnd, GWL_HWNDPARENT) == 0) &&
//       ( (((GetWindowLong(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) == 0) &&
//             GetWindow(hwnd, GW_OWNER) == 0) ||
//          ((GetWindowLong(hwnd, GWL_EXSTYLE) & WS_EX_APPWINDOW) && GetWindow(hwnd, GW_OWNER) != 0) ))

void WindowHandler::Refresh() {
   EnumerateDesktopWindows();
   GetMiceWindows();

   other_windows.clear();
   for (UINT i = 0 ; i < desktop_windows.size() ; ++i) {
      HWND hwnd = desktop_windows[i];
      if (!(IsProgramWindow(hwnd) || IsMouseWindow(hwnd))) {
         other_windows.push_back(hwnd);
      }
   }
}



void WindowHandler::SetProgramWindow(HWND prog_window) {
   program_window = prog_window;
}



bool WindowHandler::IsProgramWindow(HWND hwindow) {
   return hwindow == program_window;
}



bool WindowHandler::IsTheDesktopWindow(HWND hwindow) {
   if (!hwindow) {return false;}
   return hwindow == desktop_window;
}



bool WindowHandler::IsMouseWindow(HWND hwindow) {
   for (UINT i = 0 ; i < mice_windows.size() ; ++i) {
      if (hwindow == mice_windows[i]) {
         return true;
      }
   }
   return false;
}



bool WindowHandler::IsOurWindow(HWND hwindow) {
   return (IsProgramWindow(hwindow) || IsMouseWindow(hwindow));
}



void WindowHandler::PrintWindows() {
   Refresh();
   for (UINT i = 0 ; i < desktop_windows.size() ; ++i) {
      HWND hwnd = desktop_windows[i];

      RECT wrect;
      RECT clrect;

      GetWindowRect(hwnd , &wrect);
      GetClientRect(hwnd , &clrect);


      if (IsProgramWindow(hwnd)) {
         log.Log("Program Window : %p : (%i,%i,%i,%i)\n" , (void*)hwnd , wrect.left , wrect.top , wrect.right , wrect.bottom);
      }
      else if (IsMouseWindow(hwnd)) {
         log.Log("Mouse Window : %p : (%i,%i,%i,%i)\n" , (void*)hwnd , wrect.left , wrect.top , wrect.right , wrect.bottom);
      }
      else {
         log.Log("Desktop Window : %p : (%i,%i,%i,%i)\n" , (void*)hwnd , wrect.left , wrect.top , wrect.right , wrect.bottom);
      }
   }
}
*/
