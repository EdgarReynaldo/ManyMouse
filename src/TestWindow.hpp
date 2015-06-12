



#ifndef TestWindow_HPP
#define TestWindow_HPP

class TestWindow {

private :
   ALLEGRO_DISPLAY* display;
   ALLEGRO_EVENT_QUEUE* queue;
   ALLEGRO_TIMER* timer;

   HWND hwnd;// use al_get_win_window_handle to get this from the ALLEGRO_DISPLAY*

public :
   TestWindow();
   TestWindow() :
         display(0),
         queue(0),
         hwnd(0)
   {
      
   }

   void Free();
   void Free() {
      
   }

   bool Create(int nwidth , int nheight);
   bool Create(int nwidth , int nheight) {
      Free();
      /// Assume allegro is already setup
      
   }

   void Draw();
   void Draw() {
      
   }
   
   void InputLoop();
   void InputLoop() {
      // draw
      
      // check input
      
   }
};

#endif // TestWindow_HPP
