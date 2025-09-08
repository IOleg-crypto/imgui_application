#include "Application.h"

#include "imgui.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

Application::Application() {}

Application::~Application() { ImGui::DestroyContext(); }

void Application::Init()
{
    m_Window.Init();
    std::cout << "Initialization successful!\n";
}
void Application::RunMainLoop(Application &app)
{
#if _DEBUG
    std::setlocale(LC_ALL, "C.UTF-8");
    SetConsoleOutputCP(65001);
#endif
    while (!glfwWindowShouldClose(m_Window.GetWindow()))
    {
        glfwPollEvents();
        if (glfwGetWindowAttrib(m_Window.GetWindow(), GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        DrawUI();
        m_TabManager.ShowFontWindow();

        ImGui::Render();
        int display_w, display_h;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        glfwGetFramebufferSize(m_Window.GetWindow(), &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                     clear_color.z * clear_color.w, clear_color.w);
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
