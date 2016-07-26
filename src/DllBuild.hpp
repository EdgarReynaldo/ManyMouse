



#ifndef DllBuild_HPP
#define DllBuild_HPP




#ifdef DLLBUILD
   #define MMDECLSPEC __declspec(dllexport)
#else
   #define MMDECLSPEC __declspec(dllimport)
#endif // DLLBUILD


///int __stdcall DllMain(void* hinstDLL , unsigned long fdwReason , void* lpvReserved);





   
#endif // DllBuild_HPP

