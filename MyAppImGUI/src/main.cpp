// main.cpp - for ImGUI Application with Direct3D 11 , info see: https://github.com/ocornut/imgui
#include "App/Application.h"

// Main code
int main()
{
	Application app;
	app.Init();
	app.RunMainLoop(app);
	return 0;
}
