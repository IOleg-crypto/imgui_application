// main.cpp - for ImGUI Application with Direct3D 11 , info see: https://github.com/ocornut/imgui
#include "App/Application.h"

// Main code
int main()
{
#if _DEBUG // NOLINT(clang-diagnostic-undef)
    std::setlocale(LC_ALL, "C.UTF-8");
    SetConsoleOutputCP(65001);
#endif
	Application app;
	app.Init();
	app.RunMainLoop(app);        
#if _DEBUG
    std::cout << _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    std::cout << _CrtDumpMemoryLeaks();
#endif
	CleanupDeviceD3D();

    return 0;
}










