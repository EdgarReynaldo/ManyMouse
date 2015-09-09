

#ifndef KeyboardHandle_HPP
#define KeyboardHandle_HPP


#include "AllegroAndWindows.hpp"


class KeyboardHandler {


      bool vkeys[256];

public :

//   KeyboardHandler();
   KeyboardHandler() :
         vkeys()
   {
      /// TODO INIT vkeys
   }

//   void HandleRawInput(RAWINPUT rinput);
   void HandleRawInput(RAWINPUT rinput) {

      if (rinput.header.dwType != RIM_TYPEKEYBOARD) {return;}

      RAWKEYBOARD rkb = rinput.data.keyboard;
/*
typedef struct tagRAWKEYBOARD {
  USHORT MakeCode;
  USHORT Flags;
  USHORT Reserved;
  USHORT VKey;
  UINT   Message;
  ULONG  ExtraInformation;
} RAWKEYBOARD, *PRAWKEYBOARD, *LPRAWKEYBOARD;
*/

      switch(rkb.Message) {
      case WM_KEYDOWN :
         vkeys[rkb.VKey] = true;
         break;
      case WM_KEYUP :
         vkeys[rkb.VKey] = false;
         break;
      case WM_SYSKEYDOWN :
         
         break;
      case WM_SYSKEYUP :

         break;
      default :
         break;
      }


   }


   bool IsKeyDown(unsigned char vkeycode) {return vkeys[vkeycode];}

};




#endif // KeyboardHandle_HPP





