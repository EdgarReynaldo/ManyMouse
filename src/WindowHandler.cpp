



#include "WindowHandler.hpp"

#include "VisualLogger.hpp"


#include <cstdio>


BOOL CALLBACK EnumerationProcess(HWND hwindow , LPARAM lp) {
   vector<HWND>* pwinvec = (vector<HWND>*)lp;
   if (IsWindowVisible(hwindow)) {
      pwinvec->push_back(hwindow);
   }
   return true;
}



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
   desktop_windows.clear();
   EnumWindows(EnumerationProcess , (LPARAM)(&desktop_windows));
   desktop_window = GetDesktopWindow();
}



void WindowHandler::GetMiceWindows() {
   if (mc) {
      mice_windows.clear();
      mc->GetMiceWindows(&mice_windows);
   }
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
