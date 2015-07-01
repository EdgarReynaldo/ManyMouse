#include "Hooks.hpp"



bool swallowmouse;

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
