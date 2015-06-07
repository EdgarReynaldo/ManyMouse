


#ifndef VisualLogger_HPP
#define VisualLogger_HPP


#include "AllegroAndWindows.hpp"

#include "Mutex.hpp"

#include <deque>
using std::deque;

#include <string>
using std::string;


extern int STRINGPRINTF_BUFFER_SIZE;

string StringPrintF(const char* format_str , ...);



class VisualLogger {
   
   Mutex mutex;
   
   FILE* logfile;
   int numlines;
   int linepadding;
   deque<string> log;

   bool active;

public :
   VisualLogger();
   VisualLogger(const char* logpath);
   ~VisualLogger();

   void FreeMutex();
   bool InitMutex();

   
   void DrawLog(ALLEGRO_FONT* font , ALLEGRO_COLOR textcol , int lx , int by);
   
   void Log(string s);
   void Log(const char* format_str , ...);

   void Activate(bool activate) {
      active = activate;
   }
};



extern VisualLogger log;


#endif // VisualLogger_HPP


