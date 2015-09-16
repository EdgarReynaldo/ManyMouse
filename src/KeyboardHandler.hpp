

#ifndef KeyboardHandle_HPP
#define KeyboardHandle_HPP


#include "AllegroAndWindows.hpp"

#include "VisualLogger.hpp"
using ManyMouse::log;

int GetAllegroKeyFromVKey(unsigned char vkey);

class KeyboardHandler {


      bool vkeys[256];

      ALLEGRO_EVENT_SOURCE event_source;

public :

//   KeyboardHandler();
   KeyboardHandler() :
         vkeys()
   {
      for (unsigned int c = 0 ; c < 256 ; ++c) {
         vkeys[c] = false;
      }
      /// TODO INIT vkeys
      al_init_user_event_source(&event_source);
   }

   ALLEGRO_EVENT_SOURCE* GetEventSource() {return &event_source;}

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
//bool al_emit_user_event(ALLEGRO_EVENT_SOURCE *src,
//   ALLEGRO_EVENT *event, void (*dtor)(ALLEGRO_USER_EVENT *))

      ALLEGRO_EVENT ev;
      ev.keyboard.display = 0;
      ev.keyboard.keycode = 0;
      ev.keyboard.modifiers = 0;
      ev.keyboard.repeat = false;
      ev.keyboard.source = 0;
      ev.keyboard.timestamp = al_get_time();
      ev.keyboard.type = ALLEGRO_EVENT_KEY_DOWN;
      ev.keyboard.unichar = 0;


      switch(rkb.Message) {
      case WM_KEYDOWN :
         if (!vkeys[rkb.VKey]) {
            ev.keyboard.keycode = GetAllegroKeyFromVKey(rkb.VKey);/// ???
            al_emit_user_event(&event_source , &ev , 0);
            log.Log("VKey key release %x received\n" , (int)rkb.VKey);
         }
         vkeys[rkb.VKey] = true;
         break;
      case WM_KEYUP :
         if (vkeys[rkb.VKey]) {
            ev.keyboard.type = ALLEGRO_EVENT_KEY_UP;
            ev.keyboard.keycode = GetAllegroKeyFromVKey(rkb.VKey);/// ???
            log.Log("VKey key press %x received\n" , (int)rkb.VKey);
            al_emit_user_event(&event_source , &ev , 0);
         }
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





