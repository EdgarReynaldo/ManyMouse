#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_windows.h>
#include <windowsx.h>
#include "VisualLogger.hpp"

//using ManyMouse::log;

bool passback(ALLEGRO_DISPLAY *display, UINT message, WPARAM wparam,
   LPARAM lparam, LRESULT *result, void *userdata){
   if(message==WM_LBUTTONDOWN){
        //printf("Left button down \n");
        ManyMouse::log.Log("Left button down \n");

   }
   else if(message==WM_LBUTTONUP) {
        //printf("Left button up \n");
        ManyMouse::log.Log("Left button up \n");
   }
   else if (message==WM_RBUTTONDOWN){
        //printf("Right button down \n");
        ManyMouse::log.Log("Right button down \n");
   }
   else if (message==WM_RBUTTONUP){
//        printf("Right button up \n");
        ManyMouse::log.Log("Right button up \n");
   }
   else if (message==WM_MBUTTONDOWN){
  //      printf("Middle button down \n");
        ManyMouse::log.Log("Middle button down \n");
   }
   else if (message==WM_MBUTTONUP){
//        printf("Middle button up \n");
        ManyMouse::log.Log("Middle button up \n");
   }
   else if (message==WM_MOUSEMOVE){
        //printf("Mouse is moving;   %u \n");
        //printf("%hi , %hi \n",GET_X_LPARAM(lparam),GET_Y_LPARAM(lparam));
   }
   else if (message==WM_MOUSEHOVER){
//        printf("Mouse is hovering over \n");
        ManyMouse::log.Log("Mouse is hovering \n");
   }
   else if (message==WM_MOUSEACTIVATE) {
 //       printf("Mouse is active \n");
        ManyMouse::log.Log("Mouse is active \n");
   }
    else if (message==WM_NCLBUTTONDOWN){
        //printf("Mouse Left button down on taskbar; %u \n", message);
        ManyMouse::log.Log("Mouse left button is down on taskbar \n");
    }
    else if (message==WM_NCMOUSEMOVE) {
        //printf("Mouse Movement over taskbar; %u \n", message);
        ManyMouse::log.Log("Mouse movement over taskbar \n");
    }
    else if (message==WM_NCMOUSELEAVE){
        //printf("I dont know what this is; %u \n", message);
        ManyMouse::log.Log("NC Mouse leave \n");
    }
    else if (message==WM_NCMOUSEHOVER){
//        printf("I think this is hovering \n");
        ManyMouse::log.Log("NC Mouse is hovering \n");
    }
    else if (message==WM_NCHITTEST){
        ManyMouse::log.Log("NC Hit Test \n");
    }
    else if (message==WM_SYSCOMMAND){
         ManyMouse::log.Log("System command \n");
    }
    else if (message==WM_CAPTURECHANGED){
        ManyMouse::log.Log("Capture Changed \n");
    }
    else if (message==WM_GETMINMAXINFO){
         ManyMouse::log.Log("Min and Max info \n");
    }
    else if (message==WM_ENTERSIZEMOVE){
         ManyMouse::log.Log("Enter size move \n");
    }
    else if(message==WM_SETCURSOR){
         ManyMouse::log.Log("Set Cursor \n");
    }
    else if(message==WM_CONTEXTMENU){
         ManyMouse::log.Log("Context Menu \n");
    }
    else if(message==WM_MOUSEWHEEL){
         ManyMouse::log.Log("Mouse wheel has scrolled \n");
    }
    else if(message==WM_GETICON){
         ManyMouse::log.Log("Get Icon \n");
    }
    else if (message==WM_MOUSELEAVE){
         ManyMouse::log.Log("WM Mouse leave \n");
    }
    else if(message==WM_KEYUP){
         ManyMouse::log.Log("Key up \n");
    }
   //else printf("Unknown; %u \n", message);

   return false;
   }


int main(int argc, char **argv){

   ALLEGRO_DISPLAY *display = NULL;
   ALLEGRO_EVENT_QUEUE *event_queue = NULL;



   if(!al_init()) {
      fprintf(stderr, "failed to initialize allegro!\n");
      return -1;
   }

  if(!al_install_keyboard()) {
      fprintf(stderr, "failed to initialize the keyboard!\n");
      return -1;
   }
   if(!al_init_font_addon()){
      printf("Did not load");
      return 1;
   }
   if(!al_init_ttf_addon()){
      printf("Did not load ttf");
      return 1;
   }

   display = al_create_display(640, 480);
   if(!display) {
      fprintf(stderr, "failed to create display!\n");
      return -1;
   }

   event_queue = al_create_event_queue();
   if(!event_queue) {
      fprintf(stderr, "failed to create event_queue!\n");
      al_destroy_display(display);
      return -1;
   }
   al_register_event_source(event_queue, al_get_keyboard_event_source());
   al_register_event_source(event_queue, al_get_display_event_source(display));

if(!al_win_add_window_callback(display,passback, NULL)){
    printf("Error.");
}

   al_clear_to_color(al_map_rgb(0,0,0));

   al_flip_display();
   bool redraw = true;
   ALLEGRO_FONT* font = al_load_ttf_font("verdana.ttf",-16,0);
   if(!font){
      printf("Font was not loaded.");
   }
   while(1)
   {
      ALLEGRO_EVENT ev;
      ALLEGRO_TIMEOUT timeout;
      al_init_timeout(&timeout, 0.06);

      bool get_event = al_wait_for_event_until(event_queue, &ev, &timeout);
      if(redraw){
         al_set_target_backbuffer(display);
         al_clear_to_color(al_map_rgb(0,0,0));
         ManyMouse::log.DrawLog(font,al_map_rgb(255,255,255),10,470);
         al_flip_display();
      }
      if(get_event) {

       if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
            break;
       }
       if (ev.type == ALLEGRO_EVENT_KEY_DOWN ){
                if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE){
                    break;
                    }
            }
      }

      al_clear_to_color(al_map_rgb(0,0,0));
      al_flip_display();
   }

   al_destroy_display(display);
   al_destroy_event_queue(event_queue);


   return 0;
}
