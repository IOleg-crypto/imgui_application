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

// To add custom icon

// For render
#include "DirectX/Render.h"
#include "DirectX/d3d_context.h"

#if CHECK_MEMORYALLOC
#include "Memory.h"
#endif

/*
 * Function to toggle fullscreen mode.
 * Flips the'fullscreen' flag on or off.
 */
static void ToggleFullscreen(bool &fullscreen)
{
    fullscreen = !fullscreen;
}

/*
 * Function to show the About window (info about the application).
 * Displays application name and performance statistics (ms/frame and FPS).
 *
 * Parameters:
 * - show_demo_window: reference to a flag controlling window visibility
 * - io: ImGuiIO structure providing frame timing info
 */
static void AboutWindow(bool& show_demo_window, const ImGuiIO& io)
{
    if (ImGui::Begin("##About", &show_demo_window))
    {
        ImGui::Text("The notepad made by I#Oleg");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    }
    ImGui::End();
}

/*
 * Callback function to handle dynamic buffer resizing for ImGui::InputTextMultiline.
 * This is required when using std::string with ImGui input fields.
 *
 * Parameters:
 * - data: pointer to ImGuiInputTextCallbackData, which contains info about the input buffer and user data
 *
 * Behavior:
 * - If the buffer is empty, frees memory by swapping with an empty string
 * - If the buffer grows, resizes the underlying std::string to accommodate the new text
 * - Updates the buffer pointer to point to the new memory
 */
static int InputTextCallback(ImGuiInputTextCallbackData* data)
{
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
    {
        // Retrieve our std::string pointer from UserData.
        auto* str = static_cast<std::string*>(data->UserData);

        std::cout << "BufferText : " << data->BufTextLen << "\n";

        if (data->BufTextLen == 0)
        {
            // When the text is empty, free extra memory by swapping with an empty string.
            std::string().swap(*str);
        }
        else if (data->BufTextLen + 1 > static_cast<int>(str->size()))
        {
            // Resize the string to fit the new content (+1 for null terminator)
            str->resize(data->BufTextLen + 1);
        }

        // Let ImGui know where the resized buffer is
        data->Buf = str->data();
    }
    return 0;
}

// Main code
int main()
{
#if _DEBUG // NOLINT(clang-diagnostic-undef)
    std::setlocale(LC_ALL, "C.UTF-8");
    SetConsoleOutputCP(65001);
#endif
	// Load .ico file as window icon
	HICON hIcon = static_cast<HICON>(
		LoadImageW(nullptr, L"assets/icon/icon.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE));

	if (!hIcon) {
		MessageBoxW(nullptr, L"Failed to load icon.ico", L"Error", MB_ICONERROR);
		return 1;
	}

    WNDCLASSEXW wc = {
        sizeof(wc),                                   // cbSize
        CS_HREDRAW | CS_VREDRAW,                      // style
        WndProc,                                      // lpfnWndProc
        0,                                            // cbClsExtra
        0,                                            // cbWndExtra
        GetModuleHandle(nullptr),                     // hInstance
        hIcon,                                        // hIcon
        LoadCursor(nullptr, IDC_ARROW),               // hCursor
        reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1)), // hbrBackground
        nullptr,                                      // lpszMenuName
        L"Notepad",                                   // lpszClassName
        nullptr};
    ::RegisterClassExW(&wc);
    

    // To capture current screen resolution
    auto x = static_cast<float>(GetSystemMetrics(SM_CXSCREEN));
    auto y = static_cast<float>(GetSystemMetrics(SM_CYSCREEN));

    hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST, // Transparent Layered Window
        wc.lpszClassName, L"Notepad",
        WS_POPUP, // Removes the title bar and border
        0, 40, static_cast<int>(x), static_cast<int>(y),
        nullptr, nullptr, wc.hInstance, nullptr);
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

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
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

    static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput |
        ImGuiInputTextFlags_CtrlEnterForNewLine;

	static ImGuiWindowFlags window_flags =
		ImGuiWindowFlags_MenuBar |
		ImGuiWindowFlags_HorizontalScrollbar;
    // Static variables
    static int font_size = 16;
    static int selectedTab = 0; // Keeps track of which tab is currently selected
    // TabTitle
    static std::vector<std::string> tabTitles = {"Page1"};
    static std::vector<std::string> tabContents = {""};
    // Path for function(Save file)
    static std::string pathFile;
    static std::string currentTabInfo = tabContents[selectedTab]; // To prevent more allocations

    // local variables
    bool show_another_window = false;
    static bool show_demo_window = false;
    //ImVec4 clear_color = ImVec4(0.32f, 0.60f, 0.60f, 1.00f);
    static bool theme_change = false; // Change clear color to make it more visible
    static bool hide_window = true;
	// Global flag for fullscreen toggle
	static bool fullscreen = false;
#if _DEBUG
    std::cout << "Monitor hz : " << ImGuiDirectX::GetMonitorRefreshRate() << '\n';
#endif
    // Main loop
    bool done = false;
    while (!done)
    {

        // Poll and handle messages (inputs, window resize, etc.)
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        if (g_is_resizing_or_moving)
        {
            ::Sleep(10);
        }

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
        ImGuiDirectX::Render(hwnd, x, y);
        ImGui::NewFrame();

        // Main window
		static bool prev_fullscreen = false;
		static ImGuiCond pos_cond = ImGuiCond_Appearing;
		static ImGuiCond size_cond = ImGuiCond_Appearing;

		if (fullscreen != prev_fullscreen)
		{
			pos_cond = ImGuiCond_Always;
			size_cond = ImGuiCond_Always;
		}
		else
		{
			pos_cond = ImGuiCond_Appearing;
			size_cond = ImGuiCond_Appearing;
		}

		prev_fullscreen = fullscreen;

		if (fullscreen)
		{
			ImGui::SetNextWindowPos(ImVec2(0, 0), pos_cond);
			ImGui::SetNextWindowSize(io.DisplaySize, size_cond);
			window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar;
		}
		else
		{
			ImVec2 windowSize(800, 400);
			ImVec2 centerPos((io.DisplaySize.x - windowSize.x) * 0.5f,
				(io.DisplaySize.y - windowSize.y) * 0.5f);

			ImGui::SetNextWindowPos(centerPos, pos_cond);
			ImGui::SetNextWindowSize(windowSize, size_cond);

			window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoCollapse;
		}
		if (ImGui::Begin("Notepad", &hide_window, window_flags))
        {
            // Stop program
            if (!hide_window)
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
                        if (pathFile.empty())
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
                        ToggleFullscreen(fullscreen);
                    }
                    if (ImGui::MenuItem("Exit", "Alt+F4"))
                    {
                        ::PostQuitMessage(0);
                    }
                    if (ImGui::MenuItem("Help"))
                    {
                        show_demo_window = true;
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

                constexpr size_t maxTabs = 45;
                if (tabTitles.size() > maxTabs)
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
                ToggleFullscreen(fullscreen);
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

// Helper functions

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

    UINT createDeviceFlags = 0;
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








