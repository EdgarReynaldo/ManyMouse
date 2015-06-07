


#include "Mutex.hpp"


#include <cstdio>



map<DWORD , string> thread_id_to_name;



void RegisterThread(string thread_name , DWORD thread_id) {
   thread_id_to_name[thread_id] = thread_name;
}



Mutex::Mutex() :
      al_mutex(0),
      tid_lock_count()
{
   
}



Mutex::~Mutex() {
   Free();
}



void Mutex::Free() {
   if (al_mutex) {
      al_destroy_mutex(al_mutex);
      al_mutex = 0;
   }
   tid_lock_count.clear();
}



bool Mutex::Init() {
   Free();
   al_mutex = al_create_mutex_recursive();
   return al_mutex;
}



void Mutex::Lock() {
   
   const DWORD tid = GetCurrentThreadId();
   int count = tid_lock_count[tid];
   
   (void)count;// unused for now
   
//   printf("TID = 0x%08x LC = %d Mutex::Lock : Before lock\n" , tid , count);
   
   if (al_mutex) {
      al_lock_mutex(al_mutex);
      count = ++tid_lock_count[tid];
   }

//   printf("TID = 0x%08x LC = %d Mutex::Lock : After lock\n" , tid , count);
   
}



void Mutex::Unlock() {
   
   const DWORD tid = GetCurrentThreadId();
   int count = tid_lock_count[tid];
   
   (void)count;
   
//   printf("TID = 0x%08x LC = %d Mutex::Unlock : Before unlock\n" , tid , count);
   
   if (al_mutex) {
      al_unlock_mutex(al_mutex);
      count = --tid_lock_count[tid];
   }

//   printf("TID = 0x%08x LC = %d Mutex::Unlock : After unlock\n" , tid , count);
   
}



