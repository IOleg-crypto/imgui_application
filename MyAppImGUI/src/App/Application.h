#ifndef APPLICATION_H
#define APPLICATION_H

#include "Window/Window.h"
#include "Editor/Editor.h"
#include "Editor/UIState.h"
#if !defined(_CRT_SECURE_NO_WARNINGS)
#define CRT_SECURE_NO_WARNINGS
#endif
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include <ShlObj.h>
#include <filesystem>

// Include default C++ libraries
#include <Windows.h>
#include <iostream>
#include <vector>
#ifdef _DEBUG
#include <locale>
#endif
#include <string>
#include <tchar.h>

// For file dialog
#include "FileDialog/FileDialog.h"
// For render
#include "DirectX/Render.h"
#include "DirectX/d3d_context.h"
// For memory and buffer of ImGui::InputTextMultiline
#include "Memory.h"
#include "Editor/Editor.h"


class Application {
public:
	Application() = default;
	~Application() = default;

	void Init();
	void RunMainLoop(Application &app);

	HWND GetHwnd() { return m_Hwnd; }
private:
	void DrawUI();
private:
	Window m_Window;
	TabManager m_TabManager;
	HWND m_Hwnd;

	bool m_done = false;
};
#endif