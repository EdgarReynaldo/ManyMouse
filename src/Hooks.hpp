#include <windows.h>
#include <winuser.h>
/*bool HookMouse();
bool HookMouse(){

}*/
LRESULT CALLBACK LowLevelMouseHook(int nCode, WPARAM wParam, LPARAM lParam);
void StopMouse();
void StartMouse();






