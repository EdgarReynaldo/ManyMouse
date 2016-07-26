


#ifndef FCFSMouseStrategy_HPP
#define FCFSMouseStrategy_HPP

#include <map>
using std::map;

#include "MouseStrategy.hpp"

class Mouse;

enum FCFS_STRATEGY_STATE {
   FCFS_STATE_FREE = 0,
   FCFS_STATE_PROCESSING_CLICK = 1,
   FCFS_STATE_DRAG = 2
};



class MMDECLSPEC FCFSMouseStrategy : public MouseStrategy {

   FCFS_STRATEGY_STATE fcfs_state;

   ALLEGRO_BITMAP* fcfs_mouse_buffer;

   Mouse* grabbing_mouse;

   double mouse_drag_duration_threshold;
   double grabbing_mouse_starting_time;
   
public :
   FCFSMouseStrategy(MouseTracker* mt , WindowHandler* wh);
   virtual ~FCFSMouseStrategy();

   void FreeBuffer();

   void HandleInput(RAWINPUT input);
   virtual void HandleInput(RAWINPUT input , bool process_input);

   virtual void Reset();
   
   virtual ALLEGRO_BITMAP* GetMouseImage(Mouse* m);/// This returns a shallow temporary reference, not a new image, so copy it
   
   void ResetImages();
   void DrawPointers(FCFS_STRATEGY_STATE strategy_state);


///   void ToggleEnabled();

   virtual void SetFCFSStrategyState(FCFS_STRATEGY_STATE new_state);

   virtual MOUSE_STRATEGY GetStrategy();
};



class MMDECLSPEC TurnBasedFCFSMouseStrategy : public FCFSMouseStrategy {
   map<Mouse* , int> mouse_turn_count;
   
public :
   
};




#endif // FCFSMouseStrategy_HPP

