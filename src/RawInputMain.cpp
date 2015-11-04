

//#include <Windows.h>
//#include <Winuser.h>

// Target XP and higher, as RawInput is not supported on anything earlier
///#define WINVER 0x0501
///#define _WIN32_WINNT 0x0501

// Target Vista and higher for WM_INPUT_DEVICE_CHANGE
//#define WINVER 0x0600
//#define _WIN32_WINNT 0x0600

#include "ManyMouse.hpp"

#include "RawInputHandler.hpp"

#include <signal.h>


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

   printf("Thread ID of main function is 0x%08lx\n" , GetCurrentThreadId());

//   log.Activate(false);

   int status = SetupAllegro(atexit);
   
   if (status != 0) {
      ManyMouse::log.Log("Failed to setup allegro. status = %d.\n" , status);
      return 1;
   }

   RawInputHandler rawhandler;
   
   int windows_setup = rawhandler.SetupWindows();
   
   if (windows_setup != 0) {
      ManyMouse::log.Log("Failed to setup windows.\n");
      return 2;
   }
   
   bool raw_init = rawhandler.InitRawInfo();
   
   ManyMouse::log.Log("InitRawInfo was %s\n" , raw_init?"successful":"not successful");
   
   bool registered_devices = rawhandler.RegisterDevices(false);
   
   ManyMouse::log.Log("RegisterDevices was %s\n" , registered_devices?"successful":"not successful");
   
   

   ManyMouse::log.Activate(true);
   
   rawhandler.InputLoop();
   

   return 0;
}




