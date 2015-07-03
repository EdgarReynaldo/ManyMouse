#include "Hooks.hpp"

#include "WindowHandler.hpp"




WindowHandler* whandler = 0;

bool swallowmouse = false;



void SetWindowHandler(WindowHandler* wh) {
   whandler = wh;
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



LRESULT CALLBACK ShellHook(int nCode , WPARAM wParam , LPARAM lParam) {
   LRESULT lr = 0;
   if (nCode < 0) {
      lr = CallNextHookEx(0 , nCode , wParam , lParam);
      return lr;
   }
   if (whandler) {
      whandler->HandleShellHookInfo(nCode , wParam , lParam);
   }
   return CallNextHookEx(0 , nCode , wParam , lParam);
}




