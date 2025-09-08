#include "Application.h"

#include "imgui.h"
#include "include/GLFW/glfw3.h"
 // Rendering
 ImGui::Render();
 int display_w, display_h;
 glfwGetFramebufferSize(window, &display_w, &display_h);
 glViewport(0, 0, display_w, display_h);
 glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
 glClear(GL_COLOR_BUFFER_BIT);
 ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

 glfwSwapBuffers(window);
#include <dwmapi.h>

Application::Application() : m_Hwnd(nullptr) {}

Application::~Application()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void Application::Init()
{

    m_Window.Init();
    HWND hwnd = m_Window.GetHWND();
    m_Hwnd = hwnd;

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    std::cout << "Initialization successful!\n";
}
void Application::RunMainLoop(Application &app)
{
#if _DEBUG // NOLINT(clang-diagnostic-undef)
    std::setlocale(LC_ALL, "C.UTF-8");
    SetConsoleOutputCP(65001);
#endif
    while (!g)
    {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        DrawUI();
        m_TabManager.ShowFontWindow();

        ImGui::Render();

        const float clear_color[4] = {0.0f, 0.0f, 0.0f, 0.0f};

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(m_Window.GetWindow(), &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                     clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
#if _DEBUG
    std::cout << _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    std::cout << _CrtDumpMemoryLeaks();
#endif
}

HWND Application::GetHwnd() { return m_Hwnd; }

void Application::DrawUI()
{
    m_Window.ApplyFullscreenLayout(m_Window.windowFlags);
    if (ImGui::Begin("Notepad", &s_state.hideWindow, m_Window.windowFlags))
    {
        // Stop program
        if (!s_state.hideWindow)
        {
            ::PostQuitMessage(0);
        }
        m_TabManager.RenderMenuTab();
        m_TabManager.RenderInputTextField();
    }
    ImGui::End();
}
