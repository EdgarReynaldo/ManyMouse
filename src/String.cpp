


#include "String.hpp"
using std::string;
using std::wstring;
using std::vector;

#include <cstdio>



/*
   // Convert a wide Unicode string to an UTF8 string
   std::string utf8_encode(const std::wstring &wstr);

   // Convert an UTF8 string to a wide Unicode String
   std::wstring utf8_decode(const std::string &str);
   
   These are the original conversion functions from :
   
   http://stackoverflow.com/questions/215963/how-do-you-properly-use-widechartomultibyte

   Credit goes to Brian Bondy and tfinniga for giving these code examples
   
   I have made a small adapter which uses this code for our own purposes.
*/

// Convert a wide Unicode string to an UTF8 string
string utf8_encode(const std::wstring &wstr)
{
    if( wstr.empty() ) return string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    string strTo( size_needed, 0 );
    WideCharToMultiByte                  (CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// Convert an UTF8 string to a wide Unicode String
wstring utf8_decode(const std::string &str)
{
    if( str.empty() ) return wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    wstring wstrTo( size_needed, 0 );
    MultiByteToWideChar                  (CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}



#ifdef UNICODE
   string GetStlString(const String& s) {
      return utf8_encode(s);// s is really a std::wstring
   }
#else // not UNICODE
   string GetStlString(const String& s) {
      return string(s);// make a copy, s is really a std::string
   }
#endif // #ifdef UNICODE

int STRINGPRINTF_BUFFER_SIZE = 4096;


std::string StringPrintF(const char* format_str , ...) {
   char buffer[STRINGPRINTF_BUFFER_SIZE];
   va_list args;
   va_start(args , format_str);
///int vsnprintf (char * s, size_t n, const char * format, va_list arg );
   vsnprintf(buffer , STRINGPRINTF_BUFFER_SIZE , format_str , args);
   va_end(args);
   return std::string(buffer);
}



vector<string> SplitByNewlines(std::string s) {
   vector<string> lines;
   
   if (s.length() == 0) {
      lines.push_back("");
      return lines;
   }
   
   string line;
   for (unsigned int i = 0 ; i < s.length() ; ) {
      char c = s[i];
      if (c == '\r' || c == '\n') {
         
         lines.push_back(line);
         line = "";
      
         unsigned int i2 = i + 1;
         char c2 = (i2 < s.length())?s[i2]:'\0';
         if (c == '\r' && c2 == '\n') {
            ++i;
         }
      }
      else {
         line.push_back(c);
      }
      ++i;
      if (i == s.length() && line.length()) {
         lines.push_back(line);
      }
   }
   
   return lines;
}


