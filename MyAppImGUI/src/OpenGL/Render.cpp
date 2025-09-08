#include "Render.h"
#include <iostream>

// To create a swap chain(For performance and fast draw window)
int GetMonitorRefreshRate()
{
    // DEVMODE devMode = {};
    // devMode.dmSize = sizeof(DEVMODE);

    // // use EnumDisplaySettings for take options of current display
    // if (EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &devMode))
    // {
    //     return static_cast<int>(devMode.dmDisplayFrequency);
    // }
    // return 0;
}
void Render(GLFWwindow *window)
{
    if (window == nullptr)
    {
        std::cout << "Window is null" << std::endl;
        return;
    }

    
}
