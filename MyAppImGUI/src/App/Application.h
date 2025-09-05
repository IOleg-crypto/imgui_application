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

// Standard C++ and Windows includes
#include <Windows.h>
#include <iostream>
#include <vector>
#ifdef _DEBUG
#include <locale>
#endif
#include <string>
#include <tchar.h>

// File dialog support
#include "FileDialog/FileDialog.h"

// DirectX rendering support
#include "DirectX/Render.h"
#include "DirectX/d3d_context.h"

// Memory handling for ImGui input buffers
#include "Memory.h"

/**
 * @brief Main application class for the ImGui Notepad app.
 *
 * Responsible for initializing the UI, managing the event loop, rendering,
 * and holding core components such as the main window and tab manager.
 */
class Application {
public:
    /**
     * @brief Default constructor.
     */
    Application();

    /**
     * @brief Destructor to clean up application resources.
     */
    ~Application();

    /**
     * @brief Initialize the application components, including ImGui, window, and rendering.
     */
    void Init();

    /**
     * @brief Start and run the main application loop.
     * @param app Reference to the application object (used for state passing).
     */
    void RunMainLoop(Application& app);

    /**
     * @brief Get the HWND (handle) of the application window.
     * @return HWND handle.
     */
    HWND GetHwnd();

private:
    /**
     * @brief Draw the ImGui-based UI. Called every frame.
     */
    void DrawUI();

private:
    Window m_Window;          ///< Main window wrapper (handles window events, fullscreen, etc.).
    TabManager m_TabManager;  ///< Manages tabs and text editing functionality.
    HWND m_Hwnd;              ///< Native window handle (copied from Window class).

private:
    bool m_done = false;      ///< Main loop exit flag.
};

#endif // APPLICATION_H
