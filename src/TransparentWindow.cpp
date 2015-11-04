

#include "TransparentWindow.hpp"

#include "VisualLogger.hpp"

using namespace ManyMouse;


bool WindowPainterCallback
   (ALLEGRO_DISPLAY* display , UINT message , WPARAM wparam , LPARAM lparam , LRESULT* lresult , void* data)
{
   
   (void)display;
   (void)wparam;
   (void)lparam;
   (void)lresult;
   
   TransparentWindow* transparent_window = (TransparentWindow*)data;
   
   assert(transparent_window);
   assert(transparent_window->ready);
   
   HWND window = transparent_window->GetWindowHandle();
   
   if (message == WM_PAINT) {
//      ManyMouse::log.Log("WM_PAINT called for display %p.\n" , display);
      PAINTSTRUCT ps;
      BeginPaint(window , &ps);
///      transparent_window->dib_buffer.BlendBufferToWindowDC();
      transparent_window->dib_buffer.BlitBufferToWindowDC();
      EndPaint(window , &ps);
      return true;
   }
   
   return false;
}



void TransparentWindow::DrawImageToDIB() {
   
   if (!image) {
      ManyMouse::log.Log("TransparentWindow::DrawImageToDIB() : image is NULL\n");
      return;
   }

   dib_buffer.Flush();

   DrawBitmapToDIB(dib_buffer , image);
   
   dib_buffer.Flush();
}



void TransparentWindow::QueuePaintMessage() {
   if (window) {
      RECT clrect;
      GetClientRect(window , &clrect);
      InvalidateRect(window , &clrect , true);
      UpdateWindow(window);
   }
}



TransparentWindow::~TransparentWindow() {
   CloseTheWindow();
}



void TransparentWindow::CloseTheWindow() {
   
   dib_buffer.Free();
   
   if (image) {
      al_destroy_bitmap(image);
      image = 0;
   }
   
   if (display) {
      al_destroy_display(display);
      display = 0;
   }
   
   x = y = 0;
   w = h = -1;
   window = 0;
//   winhdc = 0;

   ready = false;

}



bool TransparentWindow::CreateTheWindow(ALLEGRO_BITMAP* img , COLORREF transparent_color , int width , int height) {
   
   CloseTheWindow();
   
   trans_color = transparent_color;
   
   if ((width == -1) || (height == -1)) {
      if (img) {
         width = al_get_bitmap_width(img);
         height = al_get_bitmap_height(img);
      }
      else {
         width = 0;
         height = 0;
      }
   }
   
   if ((width < 1 || height < 1)) {
      ManyMouse::log.Log("TransparentWindow::CreateTheWindow - Negative dimensions passed to Create call.\n");
      return false;
   }
   
   al_hold_bitmap_drawing(false);
   
   al_set_target_bitmap(NULL);
   al_set_new_display_flags(ALLEGRO_FRAMELESS | ALLEGRO_WINDOWED);
///   al_set_new_display_flags(ALLEGRO_FRAMELESS | ALLEGRO_WINDOWED | ALLEGRO_OPENGL);

   if (!(display = al_create_display(width , height))) {
      ManyMouse::log.Log("TransparentWindow::CreateTheWindow - Could not create the display.\n");
      CloseTheWindow();
      return false;
   }
   
   image = al_create_bitmap(al_get_bitmap_width(img) , al_get_bitmap_height(img));
   
   if (!image) {
      CloseTheWindow();
      return false;
   }
   
   al_set_blender(ALLEGRO_ADD , ALLEGRO_ONE , ALLEGRO_ZERO);
   al_set_target_bitmap(image);
   al_draw_bitmap(img , 0 , 0 , 0);
   
   al_get_window_position(display , &x , &y);
   
   w = width;
   h = height;
   
   window = al_get_win_window_handle(display);
   
   SetParent(window , NULL);
   
   /// Using HWND_TOPMOST has no effect on the focus, except in the initial call
   if (!SetWindowPos(window , HWND_TOPMOST , 0 , 0 , -1 , -1 , SWP_NOMOVE | SWP_NOSIZE)) {
      ManyMouse::log.Log("TransparentWindow::CreateTheWindow - Could not make window a topmost window.\n");
      CloseTheWindow();
      return false;
   }
   
   if (!BringWindowToTop(window)) {
      ManyMouse::log.Log("TransparentWindow::CreateTheWindow - Could not bring window to top.\n");
      CloseTheWindow();
      return false;
   }
   
   if (0 == SetWindowLong(window , GWL_EXSTYLE , WS_EX_LAYERED)) {
      ManyMouse::log.Log("TransparentWindow::CreateTheWindow - Could not set WS_EX_LAYERED style attribute.\n");
      CloseTheWindow();
      return false;
   }
//*
   if (!SetLayeredWindowAttributes(window , trans_color , 255 , LWA_COLORKEY | LWA_ALPHA)) {
      ManyMouse::log.Log("TransparentWindow::CreateTheWindow - Could not set layered window attributes.\n");
      CloseTheWindow();
      return false;
   }
//*/




   if (!dib_buffer.Create(window)) {
      ManyMouse::log.Log("TransparentWindow::CreateTheWindow - Failed to create DIB buffer for window %p.\n" , (void*)window);
      CloseTheWindow();
      return false;
   }
   
   dib_buffer.ClearToColor(trans_color);
   
   
   al_set_target_bitmap(image);
   
   if (image) {
      DrawImageToDIB();
   }
   
///   dib_buffer.BlendBufferToWindowDC();
   dib_buffer.BlitBufferToWindowDC();
   
   if (!AddWindowCallback(WindowPainterCallback , this)) {
      ManyMouse::log.Log("TransparentWindow::CreateTheWindow - Failed to add the window painter callback.\n");
      CloseTheWindow();
      return false;
   }
   
   ready = true;
   
   QueuePaintMessage();

   ManyMouse::log.Log("TransparentWindow::CreateTheWindow - Created transparent window successfully.\n");
   
   
   return true;
}



void TransparentWindow::PaintTheWindow() {
   if (!ready) {
      ManyMouse::log.Log("TransparentWindow::PaintTheWindow - paint called when ready is false.\n");
   }
   else {
      DrawImageToDIB();
//      dib_buffer.BlendBufferToWindowDC();
      QueuePaintMessage();
   }
}



bool TransparentWindow::SetWindowImage(ALLEGRO_BITMAP* img) {
   
/*
   if (image) {
      al_destroy_bitmap(image);
      image = 0;
   }
*/   
   if (img) {
      int nw = al_get_bitmap_width(img);
      int nh = al_get_bitmap_height(img);
      if ((nw != w) || (nh != h)) {
         if (!CreateTheWindow(img)) {
            return false;
         }
      }
      else {
         al_set_target_backbuffer(display);
         al_set_blender(ALLEGRO_ADD , ALLEGRO_ONE , ALLEGRO_ZERO);
         al_set_target_bitmap(image);
         al_draw_bitmap(img , 0 , 0 , 0);
      }
/*
      al_set_target_backbuffer(display);
      image = al_create_display(nw,nh);
      al_set_blender(ALLEGRO_ADD , ALLEGRO_ONE , ALLEGRO_ZERO);
      al_set_target_bitmap(image);
      al_draw_bitmap(img , 0 , 0);
*/
      DrawImageToDIB();
   }
   else {
      if (image) {
         al_destroy_bitmap(image);
         image = 0;
      }
      dib_buffer.ClearToColor(trans_color);
   }
   
   if (ready) {
///      dib_buffer.BlendBufferToWindowDC();
      dib_buffer.BlitBufferToWindowDC();
      QueuePaintMessage();
   }
   
   return true;
}



void TransparentWindow::SetWindowPosition(int xpos , int ypos) {
   if (display) {
      al_set_window_position(display , xpos,ypos);
      al_get_window_position(display , &x , &y);
   }
}



void TransparentWindow::SetWindowTitle(const char* title) {
   if (display) {
      al_set_window_title(display , title);
   }
}



bool TransparentWindow::AddWindowCallback(WINDOW_CALLBACK cb , void* cb_data) {
   if (!display) {return false;}
   return al_win_add_window_callback(display , cb , cb_data);
}



void TransparentWindow::UseAlphaDrawing(bool use_alpha) {
   dib_buffer.UseAlphaDrawing(use_alpha);
}



ALLEGRO_DISPLAY* TransparentWindow::GetAllegroDisplay() {
   return display;
}



HWND TransparentWindow::GetWindowHandle() {
   return window;
}



bool TransparentWindow::Ready() {return ready;}



