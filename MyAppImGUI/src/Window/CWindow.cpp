#include "CWindow.h"
#include "d3d_context.h"

namespace ImGuiNotepad {
    CWindow::CWindow(WNDCLASSEXW& wc, HICON& hIcon, HWND& hwnd, const float& x, const float& y) : m_hIcon(hIcon) , m_hwnd(hwnd) , m_wc(wc) , m_res{x, y} {
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
        0, 40, static_cast<int>(m_res.x), static_cast<int>(m_res.y),
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
	void CWindow::PollMessage(bool& done) const
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
	}

	void CWindow::HandleResize(IDXGISwapChain* swapChain)
	{
		if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
		{
			CleanupRenderTarget();
			swapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
			g_ResizeWidth = g_ResizeHeight = 0;
			CreateRenderTarget();
		}
	}
	void CWindow::HandleOcclusion(bool& g_SwapChainOccluded, IDXGISwapChain* swapChain) const
	{
		if (g_SwapChainOccluded && swapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
		{
			::Sleep(10);
			return;
		}
		g_SwapChainOccluded = false;
	}
   

	void CWindow::ApplyFullscreenLayout(bool &IsFullscreen, const ImGuiIO& io, ImGuiWindowFlags& outFlags)
	{
		static bool prevFullscreen = false;

		ImGuiCond posCond = ImGuiCond_Appearing;
		ImGuiCond sizeCond = ImGuiCond_Appearing;

		if (IsFullscreen != prevFullscreen) {
			posCond = ImGuiCond_Always;
			sizeCond = ImGuiCond_Always;
		}

		prevFullscreen = IsFullscreen;

		if (IsFullscreen) {
			ImGui::SetNextWindowPos(ImVec2(0, 0), posCond);
			ImGui::SetNextWindowSize(io.DisplaySize, sizeCond);
			outFlags = ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_MenuBar;
		}
		else {
			ImVec2 windowSize(800, 400);
			ImVec2 centerPos((io.DisplaySize.x - windowSize.x) * 0.5f,
				(io.DisplaySize.y - windowSize.y) * 0.5f);

			ImGui::SetNextWindowPos(centerPos, posCond);
			ImGui::SetNextWindowSize(windowSize, sizeCond);
			outFlags = ImGuiWindowFlags_MenuBar |
				ImGuiWindowFlags_HorizontalScrollbar |
				ImGuiWindowFlags_NoCollapse;
		}
	}

}
