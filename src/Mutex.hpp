

#ifndef Mutex_HPP
#define Mutex_HPP


#include "AllegroAndWindows.hpp"

#include <string>
using std::string;

#include <map>
using std::map;


void RegisterThread(string thread_name , DWORD thread_id);






class Mutex {

   ALLEGRO_MUTEX* al_mutex;
   
   map<DWORD , UINT> tid_lock_count;
   
   
public :
   
   Mutex();
   ~Mutex();
   
   void Free();
   bool Init();
   void Lock();
   void Unlock();

};



#endif // Mutex_HPP


