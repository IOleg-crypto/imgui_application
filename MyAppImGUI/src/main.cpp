#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>
#include <commdlg.h> // Include Windows common dialogs header
#include <shobjidl_core.h> // Include namespace for IFileOpenDialog>
#include <ShlObj.h>
#include <Shlwapi.h>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>

// Data
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static bool g_SwapChainOccluded = false;
static UINT g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;
static HWND hwnd = nullptr; // Global variable for window handle
static bool fullscreen = false; // Toggle for fullscreen mode

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

const wchar_t* ShowSaveFileDialog(const char* text)
{
    IFileSaveDialog* pFileSave = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

    if (SUCCEEDED(hr))
    {
        DWORD dwFlags;
        hr = pFileSave->GetOptions(&dwFlags);
        if (SUCCEEDED(hr))
        {
            hr = pFileSave->SetOptions(dwFlags | FOS_FORCEFILESYSTEM);
        }

        hr = pFileSave->Show(NULL);
        if (SUCCEEDED(hr))
        {
            IShellItem* pItem = nullptr;
            hr = pFileSave->GetResult(&pItem);
            if (SUCCEEDED(hr))
            {
                PWSTR pszFilePath = nullptr;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                if (SUCCEEDED(hr))
                {
                    // Copy the file path to a static buffer (for simplicity).
                    static wchar_t szFilePath[260];
                    wcscpy_s(szFilePath, pszFilePath);

                    // Write the text to the selected file.
                     // Convert wchar_t path to a narrow string.
                    std::wstring wideFilePath(szFilePath);
                    std::string filePath(wideFilePath.begin(), wideFilePath.end());

                    // Write the text to the selected file.
                    std::ofstream outFile(filePath, std::ios::out | std::ios::binary);
                    if (outFile.is_open())
                    {
                        outFile.write(text, strlen(text));
                        outFile.close();
                    }
                }
                pItem->Release();
            }
        }
        pFileSave->Release();
    }
    return nullptr;
}
const wchar_t* ShowOpenFileDialog(char* buffer, size_t bufferSize)
{
    IFileOpenDialog* pFileOpen = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

    if (SUCCEEDED(hr))
    {
        DWORD dwFlags;
        hr = pFileOpen->GetOptions(&dwFlags);
        if (SUCCEEDED(hr))
        {
            hr = pFileOpen->SetOptions(dwFlags | FOS_FORCEFILESYSTEM);
        }

        hr = pFileOpen->Show(NULL);
        if (SUCCEEDED(hr))
        {
            IShellItem* pItem = nullptr;
            hr = pFileOpen->GetResult(&pItem);
            if (SUCCEEDED(hr))
            {
                PWSTR pszFilePath = nullptr;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                if (SUCCEEDED(hr))
                {
                    // Copy the file path to a static buffer (for simplicity).
                    static wchar_t szFilePath[260];
                    wcscpy_s(szFilePath, pszFilePath);

                    // Convert wchar_t path to a narrow string.
                    std::wstring wideFilePath(szFilePath);
                    std::string filePath(wideFilePath.begin(), wideFilePath.end());

                    // Read the content of the selected file.
                    std::ifstream inFile(filePath, std::ios::in | std::ios::binary);
                    if (inFile.is_open())
                    {
                        inFile.read(buffer, bufferSize - 1);
                        buffer[inFile.gcount()] = '\0'; // Null-terminate the buffer
                        inFile.close();
                    }
                    CoTaskMemFree(pszFilePath);
                    pItem->Release();
                    pFileOpen->Release();
                    return szFilePath;
                }
                pItem->Release();
            }
        }
        pFileOpen->Release();
    }
    return nullptr;
}


// Global variables for device and context (assuming they are defined somewhere)
extern ID3D11Device* g_pd3dDevice;
extern ID3D11DeviceContext* g_pd3dDeviceContext;

void ApplyFont(int new_font_size , static char fontPath[])
{
    static std::filesystem::path font = fontPath;
    ImGuiIO& io = ImGui::GetIO();
    ImFontConfig config;

    io.Fonts->Clear();  // Clear existing fonts
    io.Fonts->AddFontFromFileTTF(font.string().c_str(), new_font_size, &config);
    io.Fonts->Build();

    // Reset device objects to apply the new font
    ImGui_ImplDX11_InvalidateDeviceObjects();
    ImGui_ImplDX11_CreateDeviceObjects();
}

void ShowFontWindow(bool& show_font_window, int font_size , static char fontPath[])
{
    static bool font_size_changed = false;
    static int new_font_size = font_size;

    ApplyFont(font_size , fontPath);
    if (show_font_window)
    {       
        if (font_size_changed)
        {
            font_size = new_font_size;  // Update the font size
            font_size_changed = false;  // Reset the flag
        }
    }
}


// Main code
int main(int, char**)
{
    // Create application window
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
    ::RegisterClassExW(&wc);
    hwnd = ::CreateWindowW(wc.lpszClassName, L"Notepad", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

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
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Our state
    bool show_another_window = false;
    static bool show_demo_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.60f, 0.60f, 1.00f);
    static bool theme_change = false; // Change clear color to make it more visible
    //ImGuiStyle& style = ImGui::GetStyle();
    //style.Colors[ImGuiCol_WindowBg] = ImVec4(222, 0, 0, 255); // Set window background color

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

        //ImGui::SetNextWindowSize(ImVec2(1000, 700), ImGuiCond_Always);
        const float sizeScalar = 1.5f; // Render a higher quality font texture for when we want to size up the font
        static int font_size = 10;
        static bool show_font_window = false;
        //bool show_font_window = true;
        //float font_size = 14.0f;


        //ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        // Main window
        static bool read_only = false;
        static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
        static char text[1024 * 16] =
            "Type here...";
        //ImGui::SetWindowFontScale(font_size);
        ImGui::Begin("##Window Name", nullptr, ImGuiWindowFlags_NoTitleBar || ImGuiWindowFlags_NoResize || ImGuiWindowFlags_NoMove || ImGuiWindowFlags_NoCollapse);
        static int selectedTab = 0;  // Keeps track of which tab is currently selected
        static std::vector<std::string> tabTitles = { "Page 1" };
        static std::vector<std::string> tabContents = { "Content for Tab 1" }; //always set static
        static char path[256] = "C://Windows//Fonts//consola.ttf";

        if (ImGui::Button("Add page"))
        {
            // Add a new tab with default content
            tabTitles.push_back("Page" + std::to_string(tabTitles.size() + 1));
            tabContents.push_back("Content for Tab " + std::to_string(tabTitles.size()));
            selectedTab = tabTitles.size() - 1; // Select the new tab
            if (selectedTab >= 30)
            {
				tabTitles.erase(tabTitles.begin() + 30, tabTitles.end());
				tabContents.erase(tabContents.begin() + 30, tabContents.end());
            }
        }

        ImGui::BeginTabBar("MyTabBar");

        for (size_t i = 0; i < tabTitles.size(); ++i)
        {
            if (ImGui::BeginTabItem(tabTitles[i].c_str()))
            {
                // Display the content for the selected tab
                ImGui::Text("%s", tabContents[i].c_str());
                // Optionally use ImGui::InputTextMultiline if you want editable content
                ImGui::InputTextMultiline("##InputText", tabContents[i].data(), IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 48), ImGuiInputTextFlags_AllowTabInput);
                ImGui::EndTabItem();
            }
        }

            //menu
            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("Menu"))
                {
                    if (ImGui::MenuItem("ReadOnly", "Ctrl+M", &read_only))
                    {
                        read_only = !read_only;
                        if (read_only)
                            flags |= ImGuiInputTextFlags_ReadOnly;
                        else
                            flags &= ~ImGuiInputTextFlags_ReadOnly;
                    }
                    ImGui::Separator();
                    if (ImGui::MenuItem("Save file dialog", "Ctrl+S"))
                    {
                        ShowSaveFileDialog(text);
                    }
                    ImGui::Separator();
                    if (ImGui::MenuItem("Open file dialog", "Ctrl+O"))
                    {
                        ShowOpenFileDialog(text, sizeof(text));
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
                    if (ImGui::MenuItem("Help"))
                    {
                        show_demo_window  = true;
                    }
                    if (ImGui::MenuItem("Theme light/dark"), "CTRL+R", &theme_change)
                    {
						theme_change = !theme_change;
						if (theme_change)
							ImGui::StyleColorsLight();
						else
							ImGui::StyleColorsDark();
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Font and size"))
                {
                    if (ImGui::MenuItem("Font"))
                    {               
                       show_font_window = true;
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
        
            
           // ImGui::InputTextMultiline("##source", NULL, 1024, ImVec2(1000, 300), ImGuiInputTextFlags_AllowTabInput);
            //ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 48), flags);
			ImGui::Separator();
            if (ImGui::RadioButton("ReadOnly", &read_only))
            {
                read_only = !read_only;
                if (read_only)
                    flags |= ImGuiInputTextFlags_ReadOnly;
                else
                    flags &= ~ImGuiInputTextFlags_ReadOnly;
            }

        }
        ImGui::End();
        //keyboard shortcuts
       
        if(ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_M))
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
			ShowSaveFileDialog(text);
        }
        if (ImGui::IsKeyPressed(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_R))
        {
            theme_change = !theme_change;
			if (theme_change)
				ImGui::StyleColorsLight();
			else
				ImGui::StyleColorsDark();
        }
        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }
        if (show_demo_window == true)
        {
            
            ImGui::SetWindowSize(ImVec2(200, 100) ,ImGuiCond_Once);
            if (ImGui::Begin("Information") , &show_demo_window , ImGuiWindowFlags_AlwaysAutoResize)
            {
                ImGui::Text("The notepad made by I#Oleg");
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            }
            ImGui::End();
        }
        if (show_font_window == true)
        {
            if (ImGui::Begin("##Font", &show_font_window, ImGuiWindowFlags_AlwaysAutoResize)) {
                if (ImGui::SliderInt("Size", &font_size, font_size, 30))
                { 
                    ShowFontWindow(show_font_window, font_size , path);

                }
                if (ImGui::IsKeyPressed(ImGuiKey_Keypad2))
                {
                    ShowFontWindow(show_font_window, font_size , path);
                    font_size--;
                }
                if (ImGui::IsKeyPressed(ImGuiKey_Keypad1))
                {
                    ShowFontWindow(show_font_window, font_size , path);
                    font_size++;
                }
                if (font_size >= 30 || font_size <= 10)
                {
                    font_size = 10;
                }
                if (ImGui::InputText("Path", path, IM_ARRAYSIZE(path), ImGuiInputTextFlags_EnterReturnsTrue))
                {
					ShowFontWindow(show_font_window, font_size , path);
                }
            }
            ImGui::End();
        }


        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Present
        HRESULT hr = g_pSwapChain->Present(1, 0);   // Present with vsync
        g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
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
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
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
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    case WM_HOTKEY:
        if (wParam == VK_F5)
        {
            ToggleFullscreen();
        }
    }

    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}


