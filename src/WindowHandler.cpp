


#include "Mouse.hpp"

#include "WindowHandler.hpp"

#include "VisualLogger.hpp"


#include <cstdio>


BOOL CALLBACK EnumerationProcess(HWND hwindow , LPARAM lp) {
   deque<HWND>* pwin = (deque<HWND>*)lp;
   if (IsWindowVisible(hwindow)) {
      pwin->push_back(hwindow);
   }
   return true;
}




void WindowHandler::GetMiceWindows() {
   if (mc) {
      mice_windows.clear();
      mc->GetMiceWindows(&mice_windows);
   }
}



void WindowHandler::EnumerateWindows() {
   
   ClearWindowInfo();
   
   GetMiceWindows();
   
   EnumWindows(EnumerationProcess , (LPARAM)(&hwnds_zorder));
   desktop_window = GetDesktopWindow();
   shell_window = GetShellWindow();
   taskbar_window = FindWindow("Shell_TrayWnd" , NULL);
   
   log.Log("Desktop window handle = %p\n" , desktop_window);
   log.Log("Shell   window handle = %p\n" , shell_window);
   log.Log("Taskbar window handle = %p\n" , taskbar_window);
   
   deque<HWND>::iterator dit = hwnds_zorder.begin();
   while (dit != hwnds_zorder.end()) {
      HWND hwnd = *dit;
   
      WindowInfo* pwi = new WindowInfo();
      
      pwi->SetWindowHandle(hwnd);
      if (hwnd == desktop_window) {
         pwi->SetWindowTypeString("The desktop window");
      }
      else if (hwnd == shell_window) {
         pwi->SetWindowTypeString("The shell window");
      }
      else if (hwnd == taskbar_window) {
         pwi->SetWindowTypeString("The taskbar window");
      }
      else if (hwnd == program_window) {
         pwi->SetWindowTypeString("Our program window");
      }
      else if (hwnd == test_window) {
         pwi->SetWindowTypeString("Our test window");
      }
      else if (IsMouseWindow(hwnd)) {
         for (unsigned int i = 0 ; i < mice_windows.size() ; ++i) {
            HWND hwndmouse = mice_windows[i];
            if (hwnd == hwndmouse) {
               pwi->SetWindowTypeString(StringPrintF("Our mouse window #%u" , i));
            }
         }
      }
      else {
         pwi->SetWindowTypeString("A user window");
      }
      
      window_info_map[hwnd] = pwi;
   
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
   SetOurWindows(program_window , test_window);
}



void WindowHandler::SetOurWindows(HWND program_window_handle , HWND test_window_handle) {
   program_window = program_window_handle;
   test_window = test_window_handle;
   
   map<HWND , WindowInfo*>::iterator it;
   
   if (program_window) {
      it = window_info_map.find(program_window);
      if (it != window_info_map.end()) {
         WindowInfo* pwi = it->second;
         pwi->SetWindowTypeString("Our program window : ");
      }
   }
   if (test_window) {
      it = window_info_map.find(test_window);
      if (it != window_info_map.end()) {
         WindowInfo* pwi = it->second;
         pwi->SetWindowTypeString("The test window    : ");
      }
   }
}



void WindowHandler::PrintWindowInfo() {
   
   deque<HWND>::iterator itwin = hwnds_zorder.begin();
   while (itwin != hwnds_zorder.end()) {
      
      HWND hwnd = *itwin;
      
      map<HWND , WindowInfo*>::iterator itinfo = window_info_map.find(hwnd);
      if (itinfo != window_info_map.end()) {
         WindowInfo* pwi = itinfo->second;
         log.Log(pwi->GetWindowInfoString());
         log.Log("\n");
      }
      
      ++itwin;
   }
}



bool WindowHandler::IsMouseWindow(HWND hwnd) {
   for (unsigned int i = 0 ; i < mice_windows.size() ; ++i) {
      HWND hwndmouse = mice_windows[i];
      if (hwnd == hwndmouse) {
         return true;
      }
   }
   return false;
}



HWND WindowHandler::GetWindowAtPos(int xpos , int ypos) {

   deque<HWND>::iterator it = hwnds_zorder.begin();
   while (it != hwnds_zorder.end()) {
      
      HWND hwnd = *it;
      
      RECT r;
      if (!GetWindowRect(hwnd , &r)) {
         log.Log("Failed to get window rect for hwnd %p\n" , hwnd);
      }
      
      if (RectContains(r , xpos , ypos)) {
         return hwnd;
      }
//      WindowInfo* pwi = 
      
      ++it;
   }
   
   return (HWND)0;
}




WindowInfo WindowHandler::GetWindowInfoFromHandle(HWND hwnd) {
   if (window_info_map.find(hwnd) != window_info_map.end()) {
      return *(window_info_map[hwnd]);
   }
   return WindowInfo();
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
