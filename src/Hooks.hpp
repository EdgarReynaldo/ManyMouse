

#include "AllegroAndWindows.hpp"


class WindowHandler;
class MouseController;

void SetWindowHandler(WindowHandler* wh);
void SetMouseController(MouseController* mc);


LRESULT CALLBACK LowLevelMouseHook(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MouseHook(int nCode, WPARAM wParam, LPARAM lParam);

void StopMouse();

void StartMouse();

LRESULT CALLBACK ShellHook(int nCode , WPARAM wParam , LPARAM lParam);




