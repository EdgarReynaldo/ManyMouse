



#ifndef MouseStates_HPP
#define MouseStates_HPP



enum NORMAL_MOUSE_STATE {
   NORMAL_MOUSE_INACTIVE = 0,
   NORMAL_MOUSE_ACTIVE = 1
};

#define NUM_NORMAL_MOUSE_STATES 2

enum FCFS_MOUSE_STATES {
   FCFS_MOUSE_INACTIVE = 0,
   FCFS_MOUSE_ACTIVE = 1,
   FCFS_MOUSE_GRABBING = 2,
   FCFS_MOUSE_DRAGGING = 3
};

#define NUM_FCFS_MOUSE_STATES 4

enum HEAVY_MOUSE_STATE {
   HEAVY_MOUSE_INACTIVE = 0,
   HEAVY_MOUSE_ACTIVE = 1,
   HEAVY_MOUSE_GRABBING = 2,
   HEAVY_MOUSE_NEEDS_HELPER = 3,
   HEAVY_MOUSE_HELPER_READY = 4,
   HEAVY_MOUSE_DRAGGING = 5
};

#define NUM_HEAVY_MOUSE_STATES 6






#endif // MouseStates_HPP
