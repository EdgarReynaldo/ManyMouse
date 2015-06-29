


#include "VisualLogger.hpp"

#include <cstdio>


#include "String.hpp"
using std::vector;

namespace ManyMouse {
   VisualLogger log;
};


void VisualLogger::FreeMutex() {
   mutex.Free();
}



bool VisualLogger::InitMutex() {
   return mutex.Init();
}



VisualLogger::VisualLogger() :
      mutex(),
      logfile(0),
      numlines(30),
      linepadding(4),
      log(),
      active(true)
{
   logfile = fopen("RawLog.txt" , "w");
}



VisualLogger::VisualLogger(const char* logpath) :
      mutex(),
      logfile(0),
      numlines(30),
      linepadding(4),
      log()
{
   logfile = fopen(logpath , "w");
}



VisualLogger::~VisualLogger() {
   if (logfile) {
      fclose(logfile);
   }
   FreeMutex();
}



void VisualLogger::DrawLog(ALLEGRO_FONT* font , ALLEGRO_COLOR textcol , int lx , int by) {
   if (!font) {return;}
   
//   printf("DrawLog : Before mutex lock\n");
   
   mutex.Lock();
   
//   printf("DrawLog : Inside mutex lock\n");

   deque<string> templog = log;
   
   mutex.Unlock();
   
//   printf("DrawLog : After mutex lock\n");
   
   deque<string>::iterator it = templog.begin();
   
   int fheight = al_get_font_line_height(font);
   int ypos = by - (fheight + linepadding);
   
   while (it != templog.end()) {
//void al_draw_text(
//      const ALLEGRO_FONT *font , ALLEGRO_COLOR color ,
//      float x , float y , int flags , char const *text) 
//      printf("%p " , it->c_str());
      al_draw_text(font , textcol , lx , ypos , 0 , it->c_str());
      ypos -= (fheight + linepadding);
      
      ++it;
   }
//   printf("\n");

//   printf("DrawLog : After drawing\n");

}
   


void VisualLogger::Log(string s) {
   
   if (active) {
      
      printf("%s" , s.c_str());
      
      fprintf(logfile , "%s" , s.c_str());

      fflush(logfile);
      
      vector<string> lines = SplitByNewlines(s);
      
      /// CRITICAL SECTION
      mutex.Lock();
      
      // Add new lines to log
      for (unsigned int i = 0 ; i < lines.size() ; ++i) {
         log.push_front(lines[i]);
      }
      
      // Remove excess lines from log
      if (numlines > 0) {
         int nlines = (int)log.size();
         while (numlines < nlines--) {
            log.pop_back();
         }
      }

      mutex.Unlock();
      /// END CRITICAL SECTION
      
   }
}
   


void VisualLogger::Log(const char* format_str , ...) {
   char buffer[STRINGPRINTF_BUFFER_SIZE];
   va_list args;
   va_start(args , format_str);
///int vsnprintf (char * s, size_t n, const char * format, va_list arg );
   vsnprintf(buffer , STRINGPRINTF_BUFFER_SIZE , format_str , args);
   va_end(args);
   
   Log(string(buffer));
}



void VisualLogger::Activate(bool activate) {
   active = activate;
}



void VisualLogger::Clear() {
   mutex.Lock();
   log.clear();
   mutex.Unlock();
}



