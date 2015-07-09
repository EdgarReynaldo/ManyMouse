#include "Hooks.hpp"

#include "WindowHandler.hpp"


#include <cstdio>



WindowHandler* whandler = 0;
MouseController* mouse_controller = 0;



bool swallowmouse = false;



void SetWindowHandler(WindowHandler* wh) {
   whandler = wh;
}


void SetMouseController(MouseController* mc) {
   mouse_controller = mc;
}



void StopMouse(){
   swallowmouse = true;
}



void StartMouse(){
   swallowmouse = false;
}



LRESULT CALLBACK LowLevelMouseHook(int nCode, WPARAM wParam, LPARAM lParam){
   LRESULT lr = 0;
   if(nCode < 0){
      lr = CallNextHookEx(0,nCode,wParam,lParam);
      return lr;
   }
   if(nCode == HC_ACTION){
      if(swallowmouse){
         return 1;
      }
      else{
         return CallNextHookEx(0,nCode,wParam,lParam);
      }
   }
   return 1;
}



LRESULT CALLBACK MouseHook(int nCode, WPARAM wParam, LPARAM lParam){
   LRESULT lr = 0;
   if(nCode < 0){
      lr = CallNextHookEx(0,nCode,wParam,lParam);
      return lr;
   }
   if(nCode == HC_ACTION){
      if(swallowmouse){
         return 1;
      }
      else{
         return CallNextHookEx(0,nCode,wParam,lParam);
      }
   }
   return 1;
}



LRESULT CALLBACK ShellHook(int nCode , WPARAM wParam , LPARAM lParam) {
   LRESULT lr = 0;
   if (nCode < 0) {
      lr = CallNextHookEx(0 , nCode , wParam , lParam);
      return lr;
   }
   
   lr = CallNextHookEx(0 , nCode , wParam , lParam);

/*   
   if (nCode == HSHELL_WINDOWACTIVATED) {
      if (mouse_controller) {
         HWND hwnd = (HWND)wParam;
         if (!mouse_controller->IsMouseWindow(hwnd)) {
            printf("Bringing mice to front.\n");
            mouse_controller->BringMiceToFront();
         }
      }
   }
*/   

   if (whandler) {
      whandler->HandleShellHookInfo(nCode , wParam , lParam);
   }
   
   return lr;
}




