


#ifndef VisualLogger_HPP
#define VisualLogger_HPP


#include "AllegroAndWindows.hpp"

#include "Mutex.hpp"

#include <deque>
using std::deque;

#include <string>
using std::string;





class VisualLogger {
   
   Mutex mutex;/// A log may be shared between threads, so it needs to be synchronized if you don't want crazy output
   
   FILE* logfile;
   int numlines;
   int linepadding;
   deque<string> log;

   bool active;
   bool console_output_active;
   bool file_output_active;
   bool window_output_active;
   
public :
   VisualLogger();
   VisualLogger(const char* logpath);
   ~VisualLogger();

   void FreeMutex();
   bool InitMutex();

   
   void DrawLog(ALLEGRO_FONT* font , ALLEGRO_COLOR textcol , int lx , int by);
   
   void Log(string s);
   void Log(const char* format_str , ...);

   void Activate(bool activate);
   void ActivateConsoleOutput(bool activate);
   void ActivateFileOutput(bool activate);
   void ActivateWindowOutput(bool activate);
   
   void Clear();
   
///   FILE* GetLogFile() {return logfile;}
};


namespace ManyMouse {
   extern VisualLogger log;
};

#endif // VisualLogger_HPP


