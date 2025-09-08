#include "Window.h"
#include "d3d_context.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "include/GLFW/glfw3.h"
#include <dwmapi.h>
#include <iostream>

bool Window::m_fullscreen = false;

Window::Window()
    : m_window(nullptr), m_hIcon(nullptr), m_iconPath(L"assets/icon/icon.ico"),
      m_fontPath(R"(C:\Windows\Fonts\Arial.ttf)"), m_io(new ImGuiIO())
{
}
Window::~Window()
{
    delete m_io;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Window::Init()
{
    GLFWwindow *m_window =
        glfwCreateWindow(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
                         "ImGui Notepad", nullptr, nullptr);
    if (!m_window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);
}

void Window::ToggleFullscreen() { m_fullscreen = !m_fullscreen; }

void Window::AboutWindow(bool &showDemoWindow)
{
    ImGuiIO &io = ImGui::GetIO();
    if (ImGui::Begin("##About", &showDemoWindow))
    {
        ImGui::Text("The notepad made by I#Oleg");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate,
                    io.Framerate);
    }
    ImGui::End();
}

void Window::ApplyFullscreenLayout(ImGuiWindowFlags &windowFlags)
{
    static bool prev_fullscreen = false;
    static ImGuiCond pos_cond = ImGuiCond_Appearing;
    static ImGuiCond size_cond = ImGuiCond_Appearing;

    ImGuiIO &io = ImGui::GetIO();
    if (m_fullscreen != prev_fullscreen)
    {
        pos_cond = ImGuiCond_Always;
        size_cond = ImGuiCond_Always;
    }
    else
    {
        pos_cond = ImGuiCond_Appearing;
        size_cond = ImGuiCond_Appearing;
    }

    prev_fullscreen = m_fullscreen;

    if (m_fullscreen)
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0), pos_cond);
        ImGui::SetNextWindowSize(io.DisplaySize, size_cond);
        windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar;
    }
    else
    {
        ImVec2 windowSize(420, 200);
        ImVec2 centerPos((io.DisplaySize.x - windowSize.x) * 0.f,
                         (io.DisplaySize.y - windowSize.y) * 0.5f);

        ImGui::SetNextWindowPos(centerPos, pos_cond);
        ImGui::SetNextWindowSize(windowSize, size_cond);

        windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar;
    }
}


void Window::InitImGui()
{
    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    m_io = &io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.WantCaptureMouse = true;
    io.Fonts->AddFontFromFileTTF(m_fontPath.c_str(), 20, nullptr,
                                 io.Fonts->GetGlyphRangesCyrillic());
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
}

ImGuiIO &Window::GetImGuiIO() { return *m_io; }

GLFWwindow* Window::GetWindow() { return m_window; }

void Window::setFontPath(const std::string &fontPath) { m_fontPath = fontPath; }

std::string Window::getFontPath() { return m_fontPath; }

bool Window::isFullscreen() { return m_fullscreen; }