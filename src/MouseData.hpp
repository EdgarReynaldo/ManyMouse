



#ifndef MouseData_HPP
#define MouseData_HPP




#include "MouseStates.hpp"


#include "DllBuild.hpp"


struct ALLEGRO_BITMAP;
struct ALLEGRO_FONT;

MMDECLSPEC void FreeMiceImages();
MMDECLSPEC bool LoadMiceImages();

/* Pass -1 for w or h to use default size */
///MMDECLSPEC ALLEGRO_BITMAP* CreateMouseImage(int w , int h , bool active);
///MMDECLSPEC ALLEGRO_BITMAP* DrawMouseImage(bool active);



extern MMDECLSPEC const char*     mouse_font_path;
extern MMDECLSPEC int             mouse_font_size;
extern MMDECLSPEC int             mouse_font_flags;
extern MMDECLSPEC ALLEGRO_FONT*   mouse_font;

extern MMDECLSPEC const char*     normal_mouse_paths [NUM_NORMAL_MOUSE_STATES];
extern MMDECLSPEC ALLEGRO_BITMAP* normal_mouse_images[NUM_NORMAL_MOUSE_STATES];

extern MMDECLSPEC const char*     fcfs_mouse_paths   [NUM_FCFS_MOUSE_STATES];
extern MMDECLSPEC ALLEGRO_BITMAP* fcfs_mouse_images  [NUM_FCFS_MOUSE_STATES];

extern MMDECLSPEC const char*     heavy_mouse_paths  [NUM_HEAVY_MOUSE_STATES];
extern MMDECLSPEC ALLEGRO_BITMAP* heavy_mouse_images [NUM_HEAVY_MOUSE_STATES];



#endif // MouseData_HPP
