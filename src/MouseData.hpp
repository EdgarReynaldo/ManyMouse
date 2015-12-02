



#ifndef MouseData_HPP
#define MouseData_HPP




#include "MouseStates.hpp"

class ALLEGRO_BITMAP;
class ALLEGRO_FONT;

void FreeMiceImages();
bool LoadMiceImages();

/* Pass -1 for w or h to use default size */
ALLEGRO_BITMAP* CreateMouseImage(int w , int h , bool active);
ALLEGRO_BITMAP* DrawMouseImage(bool active);



extern const char*     mouse_font_path;
extern int             mouse_font_size;
extern int             mouse_font_flags;
//extern ALLEGRO_FONT*   mouse_font;

extern const char*     normal_mouse_paths [NUM_NORMAL_MOUSE_STATES];
extern ALLEGRO_BITMAP* normal_mouse_images[NUM_NORMAL_MOUSE_STATES];

extern const char*     fcfs_mouse_paths   [NUM_FCFS_MOUSE_STATES];
extern ALLEGRO_BITMAP* fcfs_mouse_images  [NUM_FCFS_MOUSE_STATES];

extern const char*     heavy_mouse_paths  [NUM_HEAVY_MOUSE_STATES];
extern ALLEGRO_BITMAP* heavy_mouse_images [NUM_HEAVY_MOUSE_STATES];



#endif // MouseData_HPP
