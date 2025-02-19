// main.cpp - for Imgui Application with Direct3D 11  , info see: https://github.com/ocornut/imgui

#define _CRT_SECURE_NO_WARNINGS

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include <ShlObj.h>
#include <Shlwapi.h>
#include <commdlg.h> // Include Windows common dialogs header
#include <d3d11.h>
#include <shobjidl_core.h> // Include namespace for IFileOpenDialog>

// Include default C++ libraries
#include <cstring>
#include <fstream>
#include <iostream>
#include <locale>
#include <string>
#include <tchar.h>
#include <vector>

// For file dialog
#include "FileDialog/FileDialog.h"
#include "FileDialog/Font.h"

// To add custom icon

#include "../resource.h"


#if CHECK_MEMORYALLOC
#include "Memory.h"
#endif

#define MAX_LENGTH_MULTILINE 2048 * 19
#define MAX_LENGTH_PATH 256
#define MULTILINE_SIZE 32


// Data
static ID3D11Device *g_pd3dDevice = nullptr;
static ID3D11DeviceContext *g_pd3dDeviceContext = nullptr;
static IDXGISwapChain *g_pSwapChain = nullptr;
static bool g_SwapChainOccluded = false;
static UINT g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView *g_mainRenderTargetView = nullptr;
//static HWND hwnd = nullptr;     // Global variable for window handle
//static bool fullscreen = false; // Toggle for fullscreen mode
bool always_on_top = false;     // Toggle for always-on-top mode

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void ToggleFullscreen()
{
    static WINDOWPLACEMENT prevPlacement = { sizeof(prevPlacement) };
    static bool wasFullscreen = fullscreen;

    if (fullscreen)
    {
        // Restore windowed mode
        SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(hwnd, &prevPlacement);
        ShowWindow(hwnd, SW_RESTORE);
    }
    else
    {
        // Save current window placement
        GetWindowPlacement(hwnd, &prevPlacement);

        // Go fullscreen
        SetWindowLong(hwnd, GWL_STYLE, WS_POPUP);
        SetWindowPos(hwnd, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        ShowWindow(hwnd, SW_MAXIMIZE);
    }
    fullscreen = !fullscreen;
}


// Global variables for device and context (assuming they are defined somewhere)
extern ID3D11Device *g_pd3dDevice;
extern ID3D11DeviceContext *g_pd3dDeviceContext;

void AboutWindow(bool &show_demo_window, const ImGuiIO &io)
{
    if (ImGui::Begin("##About", &show_demo_window))
    {
        ImGui::Text("The notepad made by I#Oleg");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    }
    ImGui::End();
}

// Main code
// Main code
int main(void)
{
    // Create application window
    WNDCLASSEXW wc = {
        sizeof(wc),                                                // cbSize
        CS_HREDRAW | CS_VREDRAW,                                   // style
        WndProc,                                                   // lpfnWndProc
        0,                                                         // cbClsExtra
        0,                                                         // cbWndExtra
        GetModuleHandle(nullptr),                                  // hInstance
        LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_ICON1)), // hIcon
        LoadCursor(nullptr, IDC_ARROW),                            // hCursor
        (HBRUSH)(COLOR_WINDOW + 1),                                // hbrBackground
        nullptr,                                                   // lpszMenuName
        L"Notepad",                                                // lpszClassName
        LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_ICON1))  // hIconSm
    };
    ::RegisterClassExW(&wc);
    // Old window
    hwnd = ::CreateWindowW(wc.lpszClassName, L"Notepad", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);
    ///hwnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_NOACTIVATE,_T("Notepad") , NULL, WS_POPUP , 0 , 0 , 1280 , 720 , NULL, NULL, wc.hInstance, NULL);
    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, ULW_COLORKEY);
    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    // set font by default
    char path[] = "C:\\Windows\\Fonts\\Arial.ttf";
    io.Fonts->AddFontFromFileTTF(path, 20, nullptr, io.Fonts->GetGlyphRangesCyrillic());

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
    // Flags
    static bool show_font_window = false;
    static bool enterPressed = false;
    static bool read_only = false;

    static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput |
                                       ImGuiInputTextFlags_CtrlEnterForNewLine |
                                       ImGuiInputTextFlags_EnterReturnsTrue;
    // Static variables
    static int font_size = 16;
    static int selectedTab = 0; // Keeps track of which tab is currently selected
    // TabTitle
    static std::vector<std::string> tabTitles = {"Page 1"};
    static std::string currentTabInfo;

    //Resize
    static std::vector<std::string> tabContents = {u8""};

    // get screen width and height
    float x = GetSystemMetrics(SM_CXSCREEN);
    float y = GetSystemMetrics(SM_CYSCREEN);
    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Handle window being minimized or screen locked
        if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
        {
            ::Sleep(10);
            continue;
        }
        g_SwapChainOccluded = false;

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // local variables
        bool show_another_window = false;
        static bool show_demo_window = false;
        ImVec4 clear_color = ImVec4(0.32f, 0.60f, 0.60f, 1.00f);
        static bool theme_change = false; // Change clear color to make it more visible


        

        // Main window
        ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Notepad", nullptr , ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar))
        {

            if (ImGui::Button("Add page"))
            {
                // Add a new tab with default content
                tabTitles.push_back("Page" + std::to_string(tabTitles.size() + 1));
                tabContents.push_back(std::to_string(tabTitles.size()));
                selectedTab = int(tabTitles.size()) - 1; // Select the new tab
                if (selectedTab >= 30)
                {
                    tabTitles.erase(tabTitles.begin() + 30, tabTitles.end());
                    tabContents.erase(tabContents.begin() + 30, tabContents.end());
                }
            }

            ImGui::SameLine();
            ImGui::BeginTabBar("MyTabBar");

            for (size_t i = 0; i < tabTitles.size(); ++i)
            {
                if (ImGui::BeginTabItem(tabTitles[i].c_str()))
                {
                    selectedTab = i;
                    ImGui::Text("Content for %s", tabTitles[i].c_str());

                    size_t current_size = tabContents[i].size();
                    size_t required_size = current_size + 32; // Increase dynamically by chunks (e.g., 256 characters)

                    if (tabContents[i].capacity() < required_size)
                    {
                        tabContents[i].reserve(required_size);
                    }

                    currentTabInfo = tabContents[i];

                    ImVec2 available = ImGui::GetContentRegionAvail(); // Dynamic size of the input box
     
                    ImGui::InputTextMultiline("##InputText", tabContents[i].data(), MAX_LENGTH_MULTILINE, available, flags);

                    ImGui::EndTabItem();
                }
            }

            if (ImGui::RadioButton("Read Only", &read_only))
            {
                read_only = !read_only;
                if (read_only)
                    flags |= ImGuiInputTextFlags_ReadOnly;
                else
                    flags &= ~ImGuiInputTextFlags_ReadOnly;
            }

            ImGui::EndTabBar();
            ImGui::End();

            // keyboard shortcuts

            // 3. Show another simple window.
            if (show_another_window)
            {
                ImGui::Begin("Another Window", &show_another_window); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
                ImGui::Text("Hello from another window!");
                if (ImGui::Button("Close Me"))
                    show_another_window = false;
                ImGui::End();
            }
            if (show_demo_window)
            {
                AboutWindow(show_demo_window, io);
            }
            if (show_font_window)
            {
                ShowFontWindow(path, show_font_window, font_size);
            }

            if (ImGui::BeginChild("##child", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar)) {

                // menu
                if (ImGui::BeginMainMenuBar())
                {
                    if (ImGui::BeginMenu("Menu"))
                    {
                        if (ImGui::MenuItem("ReadOnly", "Ctrl+M", &read_only))
                        {
                            if (read_only)
                                flags |= ImGuiInputTextFlags_ReadOnly;
                            else
                                flags &= ~ImGuiInputTextFlags_ReadOnly;
                        }
                        ImGui::Separator();
                        if (ImGui::MenuItem("Save file dialog", "Ctrl+S"))
                        {
                            SaveFileDialog(hwnd, currentTabInfo);
                        }
                        ImGui::Separator();
                        if (ImGui::MenuItem("Open file dialog", "Ctrl+O"))
                        {
                            ShowOpenFileDialog(hwnd, currentTabInfo);
                            tabContents[selectedTab] = std::string(currentTabInfo);
                        }

                        ImGui::Separator();
                        if (ImGui::MenuItem("Remove page", "Delete"))
                        {
                            if (selectedTab >= 0 && selectedTab < tabTitles.size()) // Ensure selectedTab is within valid range
                            {
                                tabTitles.erase(tabTitles.begin() + selectedTab);
                                tabContents.erase(tabContents.begin() + selectedTab);

                                // Optionally, update the selectedTab index to a valid one after deletion
                                if (selectedTab >= tabTitles.size())
                                {
                                    selectedTab = static_cast<int>(tabTitles.size() - 1); // Move to the last tab if the deleted tab was the last one
                                }
                            }
                        }
                        ImGui::Separator();
                        if (ImGui::MenuItem("Fullscreen", "F5"))
                        {
                            ToggleFullscreen();
                        }
                        ImGui::Separator();
                        if (ImGui::MenuItem("Exit", "Alt+F4"))
                        {
                            ::PostQuitMessage(0);
                        }
                        ImGui::Separator();
                        if (ImGui::MenuItem("Help"))
                        {
                            show_demo_window = true;
                        }
                        ImGui::Separator();
                        ImGui::EndMenu();
                    }

                    if (ImGui::BeginMenu("Font and size"))
                    {
                        if (ImGui::MenuItem("Font"))
                        {
                            show_font_window = true;
                        }
                        else
                        {
                            show_font_window = false;
                        }
                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("Theme"))
                    {
                        if (ImGui::MenuItem("Theme light/dark", "CTRL+R"))
                        {
                            theme_change = !theme_change;
                            if (theme_change)
                                ImGui::StyleColorsLight();
                            else
                                ImGui::StyleColorsDark();
                        }
                        ImGui::EndMenu();
                    }
                    ImGui::Separator();
                    ImGui::EndMainMenuBar();
                }
            }
            ImGui::EndChild();

            if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_M))
            {
                read_only = !read_only;
                if (read_only)
                    flags |= ImGuiInputTextFlags_ReadOnly;
                else
                    flags &= ~ImGuiInputTextFlags_ReadOnly;
            }
            if (ImGui::IsKeyPressed(ImGuiKey_LeftAlt))
            {
                ::PostQuitMessage(0);
            }
            if (ImGui::IsKeyPressed(ImGuiKey_F5))
            {
                ToggleFullscreen();
            }
            if (ImGui::IsKeyPressed(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_S))
            {
                SaveFileDialog(hwnd, currentTabInfo);
            }
            if (ImGui::IsKeyPressed(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_R))
            {
                theme_change = !theme_change;
                if (theme_change)
                    ImGui::StyleColorsLight();
                else
                    ImGui::StyleColorsDark();
            }
            if (ImGui::IsKeyPressed(ImGuiKey_Delete))
            {
                if (selectedTab >= 0 && selectedTab < tabTitles.size()) // Ensure selectedTab is within valid range
                {
                    tabTitles.erase(tabTitles.begin() + selectedTab);
                    tabContents.erase(tabContents.begin() + selectedTab);

                    // Optionally, update the selectedTab index to a valid one after deletion
                    if (selectedTab >= tabTitles.size())
                    {
                        selectedTab = static_cast<int>(tabTitles.size() - 1); // Move to the last tab if the deleted tab was the last one
                    }
                }
            }

            // Rendering
            ImGui::Render();
            const float clear_color_with_alpha[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
            g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
            g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

            // Present
            HRESULT hr = g_pSwapChain->Present(1, 0); // Present with vsync
            g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
        }
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    // createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_0,
    };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

// Helper functions

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain)
    {
        g_pSwapChain->Release();
        g_pSwapChain = nullptr;
    }
    if (g_pd3dDeviceContext)
    {
        g_pd3dDeviceContext->Release();
        g_pd3dDeviceContext = nullptr;
    }
    if (g_pd3dDevice)
    {
        g_pd3dDevice->Release();
        g_pd3dDevice = nullptr;
    }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer = nullptr;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView)
    {
        g_mainRenderTargetView->Release();
        g_mainRenderTargetView = nullptr;
    }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);

    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != nullptr && wParam != SIZE_MINIMIZED)
        {
            g_ResizeWidth = (UINT)LOWORD(lParam);
            g_ResizeHeight = (UINT)HIWORD(lParam);
        }
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    case WM_HOTKEY:
        if (wParam == VK_F5)
        {
            ToggleFullscreen();
        }
        break;
    }

    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}