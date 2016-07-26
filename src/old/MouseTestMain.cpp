



#include "RawInputHandler.hpp"
#include "Mouse.hpp"
#include "DIBbuffer.hpp"


#include <cstdio>

//bool al_win_add_window_callback(ALLEGRO_DISPLAY *display,
//   bool (*callback)(ALLEGRO_DISPLAY *display, UINT message, WPARAM wparam,
//   LPARAM lparam, LRESULT *result, void *userdata), void *userdata)

extern const int NDISPLAYS;

class WindowTest;

bool WindowTestCallback(ALLEGRO_DISPLAY* display , UINT msg , WPARAM wp , LPARAM lp , LRESULT* lr , void* user);


const int NDISPLAYS = 5;

class WindowTest {

   vector<ALLEGRO_DISPLAY*> displays;
   int dcount;
   int width;
   int height;
   int r;
   int g;
   int b;
   
   ALLEGRO_EVENT_QUEUE* queue;
   ALLEGRO_TIMER* timer;
   
   MouseController mc;
   int new_handle;
   
public :
//   WindowTest();
   WindowTest() :
         displays(),
         dcount(0),
         width(512),
         height(384),
         r(255),
         g(255),
         b(255),
         queue(0),
         timer(0),
         mc(),
         new_handle(-1)
   {
      printf("Init returned %i\n" , Init());
      
   }
   
   ~WindowTest() {
      Shutdown();
   }
   
   void Shutdown() {
      mc.DestroyMice();
      
      for (int i = displays.size() - 1 ; i >= 0 ; i--) {
         ALLEGRO_DISPLAY*& d = displays[i];
         if (d) {
            al_destroy_display(d);
            d = 0;
         }
      }
      displays.clear();
   }
   
//   int Init();
   int Init() {
      if (!al_init()) {return 1;}
      if (!al_init_primitives_addon()) {return 1;}
      if (!al_init_image_addon()) {return 1;}
      if (!al_init_font_addon()) {return 1;}
      if (!al_init_ttf_addon()) {return 1;}

      if (!al_install_mouse()) {return 2;}
      if (!al_install_keyboard()) {return 2;}
//      if (!al_install_joystick()) {return -1;}
      
      timer = al_create_timer(1.0/60.0);
      if (!timer) {return 3;}
      
      queue = al_create_event_queue();
      if (!queue) {return 4;}
      
      al_register_event_source(queue , al_get_mouse_event_source());
      al_register_event_source(queue , al_get_keyboard_event_source());
      al_register_event_source(queue , al_get_timer_event_source(timer));
      
      if (!mc.CreateMouseImages()) {
         return 5;
      }
      
      return (CreateAnotherWindow()?0:-1);
   }
   
   
   
   bool NotComplete(){return dcount < NDISPLAYS;}
   
//   bool CreateAnotherWindow();
   bool CreateAnotherWindow() {
      
      if (dcount >= NDISPLAYS) {
         return false;
      }

      al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_FRAMELESS);
///      al_set_new_display_flags(ALLEGRO_OPENGL | ALLEGRO_WINDOWED | ALLEGRO_FRAMELESS);

      ALLEGRO_DISPLAY* d = al_create_display(width , height);
      displays.push_back(d);// push back nulls too
      ++dcount;// increment dcount anyway
      if (!d) {
         return false;
      }
      
      al_register_event_source(queue , al_get_display_event_source(d));
      
      al_clear_to_color(al_map_rgb(r,g,b));
      al_flip_display();
      
///      HWND hwindow = al_get_win_window_handle(d);
      
      
      width = (width*3)/4;
      height = (height*3)/4;
      r = (r*3)/4;
      g = (g*3)/4;
      b = (b*3)/4;

      if (!al_win_add_window_callback(d , WindowTestCallback , this)) {
         printf("Failed to add callback for %p, window # %i.\n" , this , dcount - 1);
      }
      return true;
   }
   
//   bool CreateAnotherMouse();
   bool CreateAnotherMouse() {
      RECT r;
      
      HWND desktop = GetDesktopWindow();
      
      GetWindowRect(desktop , &r);
      
      int sw = r.right - r.left;
      int sh = r.bottom - r.top;
      
      if (!mc.CreateMouse((HANDLE)(new_handle))) {
         printf("Failed to create mouse # %i\n" , -new_handle - 1);
         return false;
      }
      else {
         printf("Created mouse in WindowTest::CreateAnotherMouse.\n");
      }
      map<HANDLE , Mouse* >::iterator it = mc.dev_map.find((HANDLE)(new_handle));
      if (it != mc.dev_map.end()) {
         Mouse* m = it->second;
         m->SetPos(r.left + (float(rand())/RAND_MAX)*sw , r.top + (float(rand())/RAND_MAX)*sh);
      }
      --new_handle;
      return true;
   }
   
   void InputLoop() {
      
      bool redraw = true;
      bool quit = false;
   
      al_start_timer(timer);
   
      while (!quit) {
   
         if (redraw) {
//            al_set_target_backbuffer(
            mc.Draw();
            redraw = false;
         }

         do {
///         while (!al_is_event_queue_empty(queue)) {
            ALLEGRO_EVENT ev;
            al_wait_for_event(queue , &ev);
            if (ev.type == ALLEGRO_EVENT_TIMER) {
               redraw = true;
            }
            if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
               quit = true;
            }
            else if (ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
               quit = true;
            }
            else if (ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_G) {
               CreateAnotherWindow();
            }
            
         } while (!al_is_event_queue_empty(queue));
///         }
//         return true;
      }
      
   }
   
};

bool WindowTestCallback(ALLEGRO_DISPLAY* display , UINT msg , WPARAM wp , LPARAM lp , LRESULT* lr , void* user) {
   
   WindowTest* window_test = (WindowTest*)user;
   
   (void)display;
   (void)msg;
   (void)wp;
   (void)lp;
   (void)lr;
   
/*
   while (window_test->NotComplete()) {
      if (!window_test->CreateAnotherWindow()) {
         printf("Failed to create a window in the WindowTestCallback function.\n");
      }
      else {
         printf("Created a window in the WindowTestCallback function.\n");
      }
   }
*/   

///   window_test->CheckInput();
   
   if (msg == WM_KEYDOWN) {
      if (wp == 0x4D) {// m key
         printf("CreateAnotherMouse was %s\n" , window_test->CreateAnotherMouse()?"successful":"unsuccessful");
      }
   }
   
   if (msg == WM_LBUTTONDOWN) {
      printf("CreateAnotherWindow was %s\n" , window_test->CreateAnotherWindow()?"successful":"unsuccessful");
///      window_test->CreateAnotherWindow();
   }
   
   return false;
   
}


void AssertHandler(const char* exp , const char* file , int line , const char* func) {
   int* i = 0;
   *i = 1;
}

//void al_register_assert_handler(void (*handler)(char const *expr,
//   char const *file, int line, char const *func))

   
int main(int argc , char** argv) {
   

/*
   if (!al_init()) {return 1;}
   
   if (!al_init_primitives_addon()) {return 2;}
   
   
   al_set_new_display_flags(ALLEGRO_WINDOWED);
   
   ALLEGRO_DISPLAY* d1 = al_create_display(300,300);
   
   al_set_new_display_flags(ALLEGRO_OPENGL | ALLEGRO_WINDOWED);
   
   ALLEGRO_DISPLAY* d2 = al_create_display(150,150);
*/

/**
    if (!al_init()) {return 1;}
   
   if (!al_init_primitives_addon()) {return 1;}
   
   if (!al_init_image_addon()) {return 1;}
   
   if (!al_init_font_addon()) {return 1;}
   
   if (!al_init_ttf_addon()) {return 1;}
   
   if (!al_install_mouse()) {return 2;}
   if (!al_install_keyboard()) {return 2;}
   
   if (!al_install_joystick()) {
      return -1;
   }

//*/
//  RawInputHandler rh;
   
//   rh.SetupAllegro();
   
   WindowTest wt;
   
   al_register_assert_handler(AssertHandler);

//   al_set_new_display_flags(ALLEGRO_WINDOWED);
   
//   ALLEGRO_DISPLAY* d1 = al_create_display(300,300);
   
///   WindowTest wt;

/**   
   while (wt.NotComplete()) {
      if (!wt.CreateAnotherWindow()) {
         printf("Failed to create a window in the WindowTestCallback function.\n");
      }
      else {
         printf("Created another window.\n");
      }
   }
//*/



/**   
   MouseController mc;
   
   if (!mc.CreateMouseImages()) {
      printf("Failed to create mouse controller mouse images.\n");
   }
   
   RECT r;
   
   HWND desktop = GetDesktopWindow();
   
   GetWindowRect(desktop , &r);
   
   int sw = r.right - r.left;
   int sh = r.bottom - r.top;
   
   for (int i = 0 ; i < 3 ; ++i) {
      if (!mc.CreateMouse((HANDLE)(-i - 1))) {
         printf("Failed to create mouse # %i\n" , i);
      }
      else {
            
         printf("Created mouse in main.\n");
      }
      map<HANDLE , Mouse* >::iterator it = mc.dev_map.find((HANDLE)(-i - 1));
      if (it != mc.dev_map.end()) {
         Mouse* m = it->second;
         m->SetPos(r.left + (float(rand())/RAND_MAX)*sw , r.top + (float(rand())/RAND_MAX)*sh);
      }
   }
   
//   */

   wt.InputLoop();



/**
   al_set_new_display_flags(ALLEGRO_OPENGL | ALLEGRO_WINDOWED | ALLEGRO_FRAMELESS);
   
   const int NDISPLAYS = 10;
   
   int width = 512;
   int height = 384;
   int r = 255;
   int g = 255;
   int b = 255;
   
   ALLEGRO_DISPLAY* displays[NDISPLAYS];
   
   bool success = true;
   for (int i = 0 ; i < NDISPLAYS ; i++) {
      displays[i] = al_create_display(width , height);
      if (!displays[i]) {
         success = false;
         continue;
      }
      al_clear_to_color(al_map_rgb(r,g,b));
      al_flip_display();
      width = (width*3)/4;
      height = (height*3)/4;
      r = (r*3)/4;
      g = (g*3)/4;
      b = (b*3)/4;
   }
   
   printf("All displays created %s.\n" , success?"successfully":"unsuccessfully");
   al_rest(5.0);
   
   for (int i = 0 ; i < NDISPLAYS ; i++) {
      if (displays[i]) {
         al_destroy_display(displays[i]);
         displays[i] = 0;
      }
   }
//*/   
//   ALLEGRO_DISPLAY* d2 = al_create_display(150,150);

/*

   MouseController mc;
   
   mc.CreateMouseImages();
   
   mc.CreateMouse((HANDLE)0);
   mc.CreateMouse((HANDLE)1);
   mc.CreateMouse((HANDLE)2);
   
   al_set_target_backbuffer(d1);
   al_clear_to_color(al_map_rgb(0,0,0));
   al_flip_display();

   al_set_target_backbuffer(d2);
   al_clear_to_color(al_map_rgb(255,255,255));
   al_flip_display();

   al_rest(3.0);

*/
/*
   Mouse m1;
   
   ALLEGRO_BITMAP* bmp = CreateMouseImage(100,50,true);
   
   m1.Create(100,50);
   
   m1.SetImage(bmp);
   
//   m1.Draw();
   al_rest(5.0);
//*/   
   
   return 0;
}
