#include "Application.h"

#include "imgui.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#include <gl/GL.h>

#include <gl/gl.h>


Application::Application() {}

Application::~Application() = default;

void Application::Init()
{
#if _DEBUG
    std::setlocale(LC_ALL, "C.UTF-8");
    SetConsoleOutputCP(65001);
#endif

    m_Window.Init(); 
    m_Window.InitImGui();

    std::cout << "Initialization successful!\n";
}
void Application::Run()
{
#if _DEBUG
    std::setlocale(LC_ALL, "C.UTF-8");
    SetConsoleOutputCP(65001);
#endif
    int display_w = 1, display_h = 1;
    glfwGetFramebufferSize(m_Window.GetWindow(), &display_w, &display_h);
    
    while (!glfwWindowShouldClose(m_Window.GetWindow()))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImVec2 imguiPos = ImGui::GetWindowPos();
        ImVec2 imguiSize = ImGui::GetWindowSize();

        DrawUI(); 
        m_TabManager.ShowFontWindow();

        ImGui::Render();

        glViewport(0, 0, display_w, display_h);
        glClearColor(0.0f, 0.0f, 0.f,0.f); 
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(m_Window.GetWindow());
    }
#if _DEBUG
    std::cout << _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    std::cout << _CrtDumpMemoryLeaks();
#endif
}

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
