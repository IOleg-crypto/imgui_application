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
// Main code
int main(int, char**)
{
    // Create application window
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
    ::RegisterClassExW(&wc);
    hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX11 Example", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

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
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Our state
    bool show_another_window = false;
    static bool show_demo_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.60f, 0.60f, 1.00f); // Change clear color to make it more visible
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
       
        //ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        // Main window
        static bool read_only = false;
        static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
        static char text[1024 * 16] =
            "Type here...";
       
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
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Font and size"))
                {
                    if (ImGui::MenuItem("Font"))
                    {
                        // Handle font settings
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
        
            
           // ImGui::InputTextMultiline("##source", NULL, 1024, ImVec2(1000, 300), ImGuiInputTextFlags_AllowTabInput);
            ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MAX, ImGui::GetTextLineHeight() * 48), flags);
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
            ImGui::SetWindowSize(ImVec2(200, 100));
            if (ImGui::Begin("Information"))
            {
                ImGui::Text("The notepad made by I#Oleg");
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            }
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
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}


