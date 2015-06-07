

//#include <Windows.h>
//#include <Winuser.h>

// Target XP and higher, as RawInput is not supported on anything earlier
///#define WINVER 0x0501
///#define _WIN32_WINNT 0x0501

// Target Vista and higher for WM_INPUT_DEVICE_CHANGE
//#define WINVER 0x0600
//#define _WIN32_WINNT 0x0600

#include <cstdio>
#include <csignal>

///#include <windows.h>
///#include <winuser.h>
//#include <iostream>

#include "RawInputHandler.hpp"

#include "VisualLogger.hpp"

#include "Mouse.hpp"

/*
int main(int argc , char** argv) {
   int x = _WIN32_WINNT;
   int y = WM_INPUT;
//   std::cout << x << std::endl;
   
   printf("_WIN32_WINNT = 0x%08x\n" , x);
   printf("WM_INPUT = 0x%08x\n" , y);
   
   
   return 0;
   
}
*/

//bool WindowProcCallback(ALLEGRO_DISPLAY *display, UINT message, WPARAM wparam, LPARAM lparam, LRESULT *result, void *userdata);


void AbortHandler(int) {
   int* nll = 0;
   *nll = 1;
}

int main(int argc , char** argv) {

   (void)argc;
   (void)argv;

   if (argc > 1) {
      printf("Arguments given to program :\n");
      for (int i = 1 ; i < argc ; ++i) {
         printf("Arg # %i : (%s)\n" , i , argv[i]);
      }
      system("pause");
      return 0;
   }


   signal(SIGABRT , AbortHandler);
   signal(SIGTERM , AbortHandler);
   signal(SIGINT , AbortHandler);
//   signal(SIGQUIT , AbortHandler);


/**
   RawInputHandler rawhandler;
   
   rawhandler.SetupAllegro();
   
   rawhandler.InputLoop();
*/

   printf("Thread ID of main function is 0x%08lx\n" , GetCurrentThreadId());

//   log.Activate(false);

   RawInputHandler rawhandler;

   int ret = rawhandler.SetupAllegro();
   
   
   log.Log("SetupAllegro was %s. Return value was %d\n" , (ret == 0)?"successful":"not successful" , ret);
   
   log.Log("InitRawInfo was %s\n" , rawhandler.InitRawInfo()?"successful":"not successful");
   
   
   log.Log("RegisterDevices was %s\n" , (rawhandler.RegisterDevices())?"successful":"not successful");
   
   log.Activate(true);
   
   
   rawhandler.InputLoop();
   

   return 0;
}




