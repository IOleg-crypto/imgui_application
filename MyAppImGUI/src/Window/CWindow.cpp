#include "CWindow.h"
#include "d3d_context.h"

namespace ImGuiNotepad {
    CWindow::CWindow(WNDCLASSEXW &wc , HICON &hIcon , HWND& hwnd, const float &x , float &y) : m_x(x) , m_y(y) , m_hIcon(hIcon) , m_hwnd(hwnd) , m_wc(wc) {
         //Take parameters
    }
    void CWindow::Init() {
        // Load the window icon from a file path (wide string)
        m_hIcon = static_cast<HICON>(LoadImageW(
            nullptr,                // hInstance: null means use system default
            m_iconPath,             // icon path (should be a LPCWSTR, e.g., L"path\\to\\icon.ico")
            IMAGE_ICON,             // load an icon (can also be IMAGE_BITMAP, etc.)
            0,                      // use original width
            0,                      // use original height
            LR_LOADFROMFILE | LR_DEFAULTSIZE // load from file with default size
        ));

        // Check if the icon failed to load
        if (!m_hIcon) {
            MessageBoxW(nullptr, L"Failed to load icon.ico", L"Error", MB_ICONERROR);
            return;
        }

        // Fill the WNDCLASSEX structure used to register the window class
        m_wc = {
            sizeof(m_wc),                        // cbSize: size of the structure
            CS_HREDRAW | CS_VREDRAW,            // style: redraw on horizontal/vertical resize
            WndProc,                            // lpfnWndProc: pointer to window procedure function
            0,                                  // cbClsExtra: extra memory for class
            0,                                  // cbWndExtra: extra memory for window instance
            GetModuleHandle(nullptr),          // hInstance: handle to the application instance
            m_hIcon,                            // hIcon: application icon
            LoadCursor(nullptr, IDC_ARROW),    // hCursor: default arrow cursor
            reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), // hbrBackground: default window background color
            nullptr,                            // lpszMenuName: no menu
            L"Notepad",                         // lpszClassName: name of the window class
            nullptr                             // hIconSm: small icon (optional, can be nullptr)
        };

        if (!RegisterClassExW(&m_wc)) {
            MessageBoxW(nullptr, L"Failed to register window class", L"Error", MB_ICONERROR);
            return;
        }

        m_hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST, // Transparent Layered Window
        m_wc.lpszClassName, L"Notepad",
        WS_POPUP, // Removes the title bar and border
        0, 40, static_cast<int>(m_x), static_cast<int>(m_y),
        nullptr, nullptr, m_wc.hInstance, nullptr);

        if (!m_hwnd) {
            MessageBoxW(nullptr, L"Failed to create window", L"Error", MB_ICONERROR);
            return;
        }
    }

    void CWindow::ToggleFullscreen(bool &fullscreen) {
        fullscreen = !fullscreen;
    }

    void CWindow::AboutWindow(bool &show_demo_window, const ImGuiIO &io) {
        if (ImGui::Begin("##About", &show_demo_window))
        {
            ImGui::Text("The notepad made by I#Oleg");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        }
        ImGui::End();
    }
}
