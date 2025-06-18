// main.cpp - for ImGUI Application with Direct3D 11  , info see: https://github.com/ocornut/imgui
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
#include "FileDialog/Font.h"

// Init window
#include "Window/CWindow.h"
// Take flags
#include "Editor/Editor.h"

// For render
#include "DirectX/Render.h"
#include "DirectX/d3d_context.h"
// For memory and buffer of ImGui::InputTextMultiline
#include "Memory.h"

// Main code
int main()
{
#if _DEBUG // NOLINT(clang-diagnostic-undef)
    std::setlocale(LC_ALL, "C.UTF-8");
    SetConsoleOutputCP(65001);
#endif
    // To capture current screen resolution
    auto x = static_cast<float>(GetSystemMetrics(SM_CXSCREEN));
    auto y = static_cast<float>(GetSystemMetrics(SM_CYSCREEN));

	HICON hIcon;
    WNDCLASSEXW wc;
    ImGuiNotepad::CWindow window(wc , hIcon , hwnd , x , y);
    window.Init();

    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_COLORKEY);
    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
	ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Page Controls
    io.WantCaptureMouse = true;
    // set font by default
    char path[] = R"(C:\Windows\Fonts\Arial.ttf)";
    io.Fonts->AddFontFromFileTTF(path, 20, nullptr, io.Fonts->GetGlyphRangesCyrillic());

    // Setup Platform/Renderer backend
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
    // Flags
    static bool show_font_window = false;
    static bool read_only = false;
    static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
    // Don`t use ImGuiInputTextFlags_EnterReturnsTrue(it blocks)
    static ImGuiWindowFlags window_flags =
            ImGuiWindowFlags_MenuBar |
            ImGuiWindowFlags_HorizontalScrollbar;

    static ImGuiNotepad::EditorSettings g_EditorSettings;
    static ImGuiNotepad::TabManager g_TabManager;
    static ImGuiNotepad::WindowState g_WindowState;
    static int font_size = 16;
    static int selectedTab = 0; // Keeps track of which tab is currently selected

    static std::vector<std::string> tabTitles = {"Page1"};
    static std::vector<std::string> tabContents = {""};
    static std::string pathFile;
    static std::string currentTabInfo = tabContents[selectedTab]; // To prevent more allocations

    static bool show_info_window = false;
    ImVec4 clear_color = ImVec4(0.32f, 0.60f, 0.60f, 1.00f);
    static bool theme_change = false; // Change clear color to make it more visible
    static bool hide_window = true;
	static bool fullscreen = false;
#if _DEBUG
    std::cout << "Monitor hz : " << ImGuiDirectX::GetMonitorRefreshRate() << '\n';
#endif
    // Main loop
    bool done = false;
    while (!done)
    {

        window.PollMessage(done);
        if (done)
            break;

        if (g_is_resizing_or_moving)
        {
            ::Sleep(10);
        }
        window.HandleOcclusion(g_SwapChainOccluded , g_pSwapChain);
        window.HandleResize(g_pSwapChain);

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGuiDirectX::Render(hwnd, x, y);
        ImGui::NewFrame();

        window.ApplyFullscreenLayout(fullscreen, io, window_flags);
		if (ImGui::Begin("Notepad", &g_WindowState.hide_window, window_flags))
        {
            // Stop program
            if (!g_WindowState.hide_window)
            {
                ::PostQuitMessage(0);
            }
            // menu
            if (ImGui::BeginMenuBar()) // Connect menu bar with ImGuiWindowFlags_MenuBar
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
                    if (ImGui::MenuItem("Save file as", "Ctrl+ Left Shift + S"))
                    {
                        SaveFileDialog(hwnd, currentTabInfo, pathFile);

                        if (!pathFile.empty())
                        {
                            // Only takes the file name
                            std::filesystem::path filePath(pathFile);
                            tabTitles[selectedTab] = filePath.filename().string();
                            tabContents[selectedTab].resize(currentTabInfo.size() + 1); // +1 for null terminator
                            std::memcpy(tabContents[selectedTab].data(), currentTabInfo.c_str(), currentTabInfo.size() + 1);
                        }
                    }
                    if (ImGui::MenuItem("Save file", "Ctrl+S"))
                    {
                        // To fix bug with empty name of tab
                        if (g_TabManager.pathFile.empty())
                        {
                            pathFile = tabTitles[selectedTab];
                            MessageBoxA(hwnd, "No valid directory found", "File not saved", MB_OK);
                        }
                        else
                        {
                            SaveFile(hwnd, pathFile, currentTabInfo);
                            tabTitles[selectedTab] = std::move(pathFile);
                        }
                    }
                    if (ImGui::MenuItem("Open file", "Ctrl+O"))
                    {
                        ShowOpenFileDialog(hwnd, currentTabInfo, pathFile);
                        // To prevent add file, when tab don`t exist
                        if (!tabTitles.empty())
                        {
                            if (!pathFile.empty())
                            {
                                // Only takes the file name
                                std::filesystem::path filePath(pathFile);
                                tabTitles[selectedTab] = filePath.filename().string();
                                tabContents[selectedTab].resize(currentTabInfo.size() + 1); // +1 for null terminator
                                std::memcpy(tabContents[selectedTab].data(), currentTabInfo.c_str(), currentTabInfo.size() + 1);
                            }
                        }
                    }
                    if (ImGui::MenuItem("Remove page", "Delete"))
                    {
                        if (selectedTab >= 0 && selectedTab <= static_cast<int>(tabTitles.size())) // Ensure selectedTab is within valid range
                        {
                            tabTitles.erase(tabTitles.begin() + selectedTab);
                            tabContents.erase(tabContents.begin() + selectedTab);

                            // Optionally, update the selectedTab index to a valid one after deletion
                            if (selectedTab <= static_cast<int>(tabTitles.size()))
                            {
                                selectedTab = static_cast<int>(tabTitles.size() - 1); // Move to the last tab if the deleted tab was the last one
                            }
                        }
                    }
                    if (ImGui::MenuItem("Fullscreen", "3"))
                    {
                        window.ToggleFullscreen(fullscreen);
                        window.ApplyFullscreenLayout(g_WindowState.fullscreen, io, window_flags);
                    }
                    if (ImGui::MenuItem("Exit", "Alt+F4"))
                    {
                        ::PostQuitMessage(0);
                    }
                    if (ImGui::MenuItem("Help"))
                    {
                        show_info_window = true;
                    }
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
                ImGui::EndMenuBar();
            }

            // Add page logic
            if (ImGui::Button("Add page"))
            {
                tabTitles.emplace_back("Page" + std::to_string(tabTitles.size() + 1));
                tabContents.emplace_back();

                selectedTab = static_cast<int>(tabTitles.size()) - 1;

                if (constexpr size_t maxTabs = 45; tabTitles.size() > maxTabs)
                {
                    tabTitles.pop_back();
                }
            }

            ImGui::Separator();
            ImGui::BeginTabBar("MyTabBar");

            for (int i = 0; i < static_cast<int>(tabTitles.size()); ++i)
            {
                bool open = true;
                if (ImGui::BeginTabItem(tabTitles[i].c_str(), &open))
                {
                    selectedTab = i;
                    ImGui::Text("Content for %s", tabTitles[i].c_str());

                    ImGui::InputTextMultiline("##InputText", tabContents[i].data(), tabContents[i].capacity(),
                                              ImVec2(x, y), flags | ImGuiInputTextFlags_CallbackResize, InputTextCallback, static_cast<void *>(&tabContents[i]));

                    ImGui::EndTabItem();
                }

                // If tab is closed, remove it (using reverse iteration to avoid shifting)
                if (!open && i < static_cast<int>(tabTitles.size()))
                {
                    tabTitles[i] = std::move(tabTitles.back()); // Move last item into current position
                    tabContents[i] = std::move(tabContents.back());
                    // O(1) instead of O(n)
                    tabTitles.pop_back();
                    tabContents.pop_back();
                    tabTitles.shrink_to_fit();
                    tabContents.shrink_to_fit();
                    if ((selectedTab >= static_cast<int>(tabTitles.size())))
                    {
                        selectedTab = static_cast<int>(tabTitles.size() - 1);
                    }
                    // To avoid situation when tabTitles empty(user delete all tabs)
                    if (tabTitles.empty())
		            {
						tabTitles.emplace_back("Page" + std::to_string(tabTitles.size() + 1));
						tabContents.emplace_back();
		            }
                }
            }

            ImGui::Separator();

            if (ImGui::RadioButton("Read Only", &read_only))
            {
                read_only = !read_only;
                if (read_only)
                    flags |= ImGuiInputTextFlags_ReadOnly;
                else
                    flags &= ~ImGuiInputTextFlags_ReadOnly;
            }

            ImGui::EndTabBar();

            // keyboard shortcuts

            if (show_info_window)
            {
                window.AboutWindow(show_info_window, io);
            }
            if (show_font_window)
            {
                ShowFontWindow(path, show_font_window, font_size);
            }
            if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_O, false))
            {
                ShowOpenFileDialog(hwnd, currentTabInfo, pathFile);
            }
            if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_M, false))
            {
                read_only = !read_only;
                if (read_only)
                    flags |= ImGuiInputTextFlags_ReadOnly;
                else
                    flags &= ~ImGuiInputTextFlags_ReadOnly;
            }
#if _DEBUG  // Let it debug  , cause , you kill program
            if (ImGui::IsKeyPressed(ImGuiKey_LeftAlt, false))
            {
                ::PostQuitMessage(0);
            }
#endif
            if (ImGui::IsKeyPressed(ImGuiKey_F5))
            {
                window.ToggleFullscreen(fullscreen);
            }
            if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_LeftShift, false) && ImGui::IsKeyPressed(ImGuiKey_S, false))
            {
                SaveFileDialog(hwnd, currentTabInfo, pathFile);
            }
            if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_R, false))
            {
                theme_change = !theme_change;
                if (theme_change)
                    ImGui::StyleColorsLight();
                else
                    ImGui::StyleColorsDark();
            }
            if (ImGui::IsKeyPressed(ImGuiKey_Delete))
            {
                if (selectedTab > 0 && selectedTab < static_cast<int>(tabTitles.size())) // Ensure selectedTab is within valid range
                {
                    tabTitles.erase(tabTitles.begin() + selectedTab);
                    tabContents.erase(tabContents.begin() + selectedTab);

                    // Optionally, update the selectedTab index to a valid one after deletion
                    if (selectedTab > static_cast<int>(tabTitles.size()))
                    {
                        selectedTab = static_cast<int>(tabTitles.size() - 1); // Move to the last tab if the deleted tab was the last one
                    }
                }
            }
            if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_F, false))
            {
                SaveFile(hwnd, pathFile, currentTabInfo);
            }
            ImGui::End();


            // Rendering
            ImGui::Render();
            constexpr float clear_color_with_alpha[4] = {0.0f, 0.0f, 0.0f, 0.0f};
            g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
            g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

            // Present
            /*
             *   Present(1,0) - vsync with monitor
             *   Present(0 , 0) - unlocked fps
             */
            HRESULT hr = g_pSwapChain->Present(0, 0); // Present with vsync
            g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
        }
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(hwnd);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
#if _DEBUG
    std::cout << _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    std::cout << _CrtDumpMemoryLeaks();
#endif

    return 0;
}
bool CreateDeviceD3D(const HWND &hWnd)
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

    constexpr UINT createDeviceFlags = 0;
    // createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    constexpr D3D_FEATURE_LEVEL featureLevelArray[2] = {
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

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer = nullptr;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    /*
    *  pBackBuffer - could be nullptr
    */
    if (pBackBuffer == nullptr) {
        return;
    }
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}








