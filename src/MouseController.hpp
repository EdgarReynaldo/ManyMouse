
#ifndef MouseController_HPP
#define MouseController_HPP


#include "AllegroAndWindows.hpp"

#include "Mouse.hpp"
#include "MouseStrategy.hpp"
#include "MouseStrategyFCFS.hpp"
#include "MouseStrategyHeavy.hpp"
#include "MouseStrategyNormal.hpp"
#include "MouseTracker.hpp"

#include <vector>
using std::vector;

class MouseTracker;



#define WM_MANYMOUSE_LOAD_MICE_IMAGES   WM_APP + 1
#define WM_MANYMOUSE_FREE_MICE_IMAGES   WM_APP + 2
#define WM_MANYMOUSE_CHANGE_STRATEGY    WM_APP + 3
#define WM_MANYMOUSE_DRAW_MICE_IMAGES   WM_APP + 4

/*
class MouseTrackingInfo {
public :
   bool used;
   unsigned int id;
   HANDLE hdev;
   Mouse* mouse;

//   MouseTrackingInfo();
   MouseTrackingInfo() :
         used(false),
         id(0),
         hdev(0),
         mouse(0)
   {

   }
};



/// MouseTracker is responsible for tracking and destroying mice, but does not create them

class MouseTracker {

private :
   vector<MouseTrackingInfo> mtinfo;

   MouseTracker& operator=(const MouseTracker& mt) {(void)mt;return *this;}// disallow copying
   MouseTracker(const MouseTracker& mt) {(void)mt;}// disallow copy constructor

public :

   MouseTracker();

   unsigned int TrackNewMouse(Mouse* mouse , HANDLE hdev);// adopts ownership of Mouse*
   void StopTrackingMouse(HANDLE hdev);

   ~MouseTracker();

   void CleanUp();

   vector<unsigned int> GetIdsInUse();// duh

   Mouse* GetMouseFromHandle(HANDLE hdev);

   unsigned int NMice();// Number of mice present
   Mouse* GetMouseByIndex(unsigned int index);// call NMice() to find out which indices are valid first

   vector<Mouse*> GetMouseVector();

};
*/


/// ----------------------------------------------   MouseController class  ------------------------------------------------------------------



class MMDECLSPEC MouseController {
public:

   MouseTracker mouse_tracker;

   NormalMouseStrategy normal_strategy;
   FCFSMouseStrategy fcfs_strategy;
   HeavyMouseStrategy heavy_strategy;
//   CollaborativeMouseStrategy collaborative_strategy;

   MouseStrategy* active_strategy;
   
   MOUSE_STRATEGY current_mouse_strategy;


///   ALLEGRO_BITMAP* ms_enabled_image;
///   ALLEGRO_BITMAP* ms_disabled_image;

   bool mouse_image_enabled;

   bool mice_active;

//   bool mice_enabled;
   bool mice_shown;

   WindowHandler* window_handler;

   int mouse_device_count;

   bool mice_loaded;
   bool mice_freed;
   

   void DestroyMouse(HANDLE hDevice);
   bool CreateMouse(HANDLE hDevice);

   void DestroyMice();


public :
   MouseController(WindowHandler* wh);

   ~MouseController();

///   void FreeMouseImages();
///   bool CreateMouseImages();


   void HandleRawInput(RAWINPUT rawinput);

   void Draw();

///   void ToggleMouseImage();

   void ActivateMice(bool activate_mice);
   void ShowMice(bool show_mice);


   void GetMiceWindows(vector<HWND>* winvec);
   vector<Mouse*> GetMice();

   void SetWindowHandler(WindowHandler* handler);

   void BringMiceToFront();

   bool IsMouseWindow(HWND hwnd);

   void SetMouseStrategy(MOUSE_STRATEGY strategy);
   MOUSE_STRATEGY CurrentStrategy() {return current_mouse_strategy;}
   
   bool InitializeMice();
   void ResetStrategies();
   void FreeMice();

   
};





#endif // MouseController_HPP


