


#ifndef String_HPP
#define String_HPP


#include "AllegroAndWindows.hpp"

#include <string>
#include <vector>


#include "DllBuild.hpp"



/*

   // Convert a wide Unicode string to an UTF8 string
   std::string utf8_encode(const std::wstring &wstr)
   {
       if( wstr.empty() ) return std::string();
       int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
       std::string strTo( size_needed, 0 );
       WideCharToMultiByte                  (CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
       return strTo;
   }

   // Convert an UTF8 string to a wide Unicode String
   std::wstring utf8_decode(const std::string &str)
   {
       if( str.empty() ) return std::wstring();
       int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
       std::wstring wstrTo( size_needed, 0 );
       MultiByteToWideChar                  (CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
       return wstrTo;
   }

   These are the original conversion functions from :
   
   http://stackoverflow.com/questions/215963/how-do-you-properly-use-widechartomultibyte

   Credit goes to Brian Bondy and tfinniga for giving these code examples
   
   I have made a small adapter which uses this code for our own purposes.
*/

// Convert a wide Unicode string to an UTF8 string
MMDECLSPEC std::string utf8_encode(const std::wstring &wstr);

// Convert an UTF8 string to a wide Unicode String
MMDECLSPEC std::wstring utf8_decode(const std::string &str);


// this depends on whether or not UNICODE is defined. If so, TCHAR is WCHAR else CHAR
///typedef std::basic_string<TCHAR> String;

// this does the same thing, bc wstring is an alias for basic_string<wchar_t>
#ifdef UNICODE
   typedef std::wstring String;

#else // not UNICODE
   typedef std::string String;

#endif // #ifdef UNICODE

MMDECLSPEC std::string GetStlString(const String& s);

#define STRINGPRINTF_BUFFER_SIZE 4096

MMDECLSPEC std::string StringPrintF(const char* format_str , ...);

MMDECLSPEC std::vector<std::string> SplitByNewLines(std::string s);





#endif // String_HPP

