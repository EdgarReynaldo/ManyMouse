


#include "MouseData.hpp"


#include "AllegroAndWindows.hpp"

#include "VisualLogger.hpp"


//ALLEGRO_FONT* mouse_font = 0;

MMDECLSPEC const char* mouse_font_path = "verdana.ttf";
MMDECLSPEC int mouse_font_size = -20;
MMDECLSPEC int mouse_font_flags = ALLEGRO_TTF_MONOCHROME;




MMDECLSPEC const char* normal_mouse_paths[NUM_NORMAL_MOUSE_STATES] = {
   "Images/DaltonRedCursor2.png",
   "Images/DaltonGreenCursor2.png"
};



MMDECLSPEC ALLEGRO_BITMAP* normal_mouse_images[NUM_NORMAL_MOUSE_STATES] = {
   0,0
};



MMDECLSPEC const char* fcfs_mouse_paths[NUM_FCFS_MOUSE_STATES] = {
   "NewImages/MouseInactive2.png",
   "NewImages/MouseActive2.png",
   "NewImages/MouseGrabbing2.png",
   "NewImages/MouseDragging2.png"
};

MMDECLSPEC ALLEGRO_BITMAP* fcfs_mouse_images[NUM_FCFS_MOUSE_STATES] = {
   0,0,0,0
};



MMDECLSPEC const char* heavy_mouse_paths[NUM_HEAVY_MOUSE_STATES] = {
   "NewImages/MouseInactive2.png",
   "NewImages/MouseActive2.png",
   "NewImages/MouseGrabbing2.png",
   "NewImages/MouseNeedsHelper2.png",
   "NewImages/MouseHelperReady2.png",
   "NewImages/MouseDragging2.png"
};



MMDECLSPEC ALLEGRO_BITMAP* heavy_mouse_images[NUM_HEAVY_MOUSE_STATES] = {
   0,0,0,0,0,0
};



MMDECLSPEC void FreeMiceImages() {
   for (int i = 0 ; i < NUM_NORMAL_MOUSE_STATES ; ++i) {
      ALLEGRO_BITMAP*& image = normal_mouse_images[i];
      if (image) {
         DestroyAllegroBitmap(image);
         image = 0;
      }
   }
   for (int i = 0 ; i < NUM_FCFS_MOUSE_STATES ; ++i) {
      ALLEGRO_BITMAP*& image = fcfs_mouse_images[i];
      if (image) {
         DestroyAllegroBitmap(image);
         image = 0;
      }
   }
   for (int i = 0 ; i < NUM_HEAVY_MOUSE_STATES ; ++i) {
      ALLEGRO_BITMAP*& image = heavy_mouse_images[i];
      if (image) {
         DestroyAllegroBitmap(image);
         image = 0;
      }
   }
//   if (mouse_font) {
//      al_destroy_font(mouse_font);
//      mouse_font = 0;
//   }
}



MMDECLSPEC bool LoadMiceImages() {
   FreeMiceImages();
   bool success = true;
   for (int i = 0 ; i < NUM_NORMAL_MOUSE_STATES ; ++i) {
      ALLEGRO_BITMAP* image = al_load_bitmap(normal_mouse_paths[i]);
      if (!image) {
         success = false;
         ManyMouse::log.Log("Failed to load mouse image \"%s\"\n" , normal_mouse_paths[i]);
      }
      normal_mouse_images[i] = image;
   }
   for (int i = 0 ; i < NUM_FCFS_MOUSE_STATES ; ++i) {
      ALLEGRO_BITMAP* image = al_load_bitmap(fcfs_mouse_paths[i]);
      if (!image) {
         success = false;
         ManyMouse::log.Log("Failed to load mouse image \"%s\"\n" , fcfs_mouse_paths[i]);
      }
      fcfs_mouse_images[i] = image;
   }
   for (int i = 0 ; i < NUM_HEAVY_MOUSE_STATES ; ++i) {
      ALLEGRO_BITMAP* image = al_load_bitmap(heavy_mouse_paths[i]);
      if (!image) {
         success = false;
         ManyMouse::log.Log("Failed to load mouse image \"%s\"\n" , heavy_mouse_paths[i]);
      }
      heavy_mouse_images[i] = image;
   }
//   mouse_font = al_load_ttf_font(mouse_font_path , 20 , ALLEGRO_TTF_MONOCHROME);
//   if (!mouse_font) {
//      ManyMouse::log.Log("Failed to load mouse font at '%s'\n" , mouse_font_path);
//      success = false;
//   }
   
   
   if (!success) {
      FreeMiceImages();
   }
   return success;
}



MMDECLSPEC ALLEGRO_BITMAP* CreateMouseImage(int w , int h , bool active) {

   // Pass -1 for w or h to use default size


   ALLEGRO_BITMAP* cursor_bmp = 0;

   if (!active) {
///      cursor_bmp = al_load_bitmap("Images/CursorRed.png");
      cursor_bmp = al_load_bitmap("Images/DaltonRedCursor2.png");
///      cursor_bmp = al_load_bitmap("Images/TransMouseOverlayB.png");
   }
   else {
///      cursor_bmp = al_load_bitmap("Images/CursorGreen.png");
      cursor_bmp = al_load_bitmap("Images/DaltonGreenCursor2.png");
///      cursor_bmp = al_load_bitmap("Images/TransMouseOverlayA.png");
   }

   if (!cursor_bmp) {
      ManyMouse::log.Log("Failed to load cursor bitmap.\n");
      return 0;
   }

   if ((w == -1) || (h == -1)) {
      // Return unmodified image
      return cursor_bmp;
   }

   /// Scale image to specified width and height
   ALLEGRO_BITMAP* user_bmp = 0;

   user_bmp = CreateAllegroBitmap(w , h);
   if (!user_bmp) {
      al_destroy_bitmap(cursor_bmp);
      return 0;
   }

   al_set_blender(ALLEGRO_ADD , ALLEGRO_ONE , ALLEGRO_ZERO);
   AllegroSetTargetBitmap(user_bmp);

//void al_draw_scaled_bitmap(ALLEGRO_BITMAP *bitmap,
//   float sx, float sy, float sw, float sh,
//   float dx, float dy, float dw, float dh, int flags)

   al_draw_scaled_bitmap(cursor_bmp , 0.0 , 0.0 , al_get_bitmap_width(cursor_bmp) , al_get_bitmap_height(cursor_bmp),
                                      0.0 , 0.0 , w , h , 0);

   al_destroy_bitmap(cursor_bmp);

   return user_bmp;

}



MMDECLSPEC ALLEGRO_BITMAP* DrawMouseImage(bool active){

   ALLEGRO_BITMAP* cursor1 = 0;
   ALLEGRO_BITMAP* circle = 0;
   cursor1 = al_load_bitmap("Images/NormalCursor1.png");
   if (!cursor1) {
      ManyMouse::log.Log("Failed to load cursor bitmap.\n");
      return 0;
   }
   circle = CreateAllegroBitmap(32,32);
   if(!circle){
      DestroyAllegroBitmap(cursor1);
      return 0;
   }
   AllegroSetTargetBitmap(circle);
   al_clear_to_color(al_map_rgba(0 , 1.0*0.5 , 0 , 0.5));
   //al_set_blender(ALLEGRO_ADD , ALLEGRO_ONE , ALLEGRO_ZERO);
   al_set_blender(ALLEGRO_ADD , ALLEGRO_ONE , ALLEGRO_INVERSE_ALPHA);
   float a = 0.5;
   ALLEGRO_COLOR black = al_map_rgba_f(0 , 0 , 0 , 1);

   al_draw_filled_rectangle(0,0,16,32,black);
   ALLEGRO_COLOR color = al_map_rgba_f(1.0, 0 , 0 , a);
   if(active){
      color = al_map_rgba_f(0 , 0 , 1.0*a , a);
   }
   al_draw_filled_circle(16,16,16,color);
   //al_set_blender(ALLEGRO_ADD , ALLEGRO_ONE , ALLEGRO_INVERSE_ALPHA);
         al_draw_bitmap(cursor1,0,0,0);

   DestroyAllegroBitmap(cursor1);
   return circle;
}




