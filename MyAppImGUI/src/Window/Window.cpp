#include "Window.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>

#include <nfd.h>
#include <nfd_glfw3.h>

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <Windows.h>
#endif

#ifdef __linux__
#include <X11/Xlib.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
namespace ImGui{
bool Window::m_fullscreen = false;

Window::Window()
    : m_window(nullptr), m_iconPath(PROJECT_ROOT_DIR "/assets/icon/icon_window.png"),
      m_fontPath()
{
}

Window::~Window()
{
    // Shutdown ImGui backend first
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    // Destroy ImGui context
    ImGui::DestroyContext();
    NFD_Quit();

    delete m_io;

    // Destroy GLFW window
    if (m_window)
        glfwDestroyWindow(m_window);

    // Terminate GLFW
    glfwTerminate();
}

void Window::Init()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

    

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
#ifdef _WIN32
    m_window = glfwCreateWindow(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
                                "ImGui Notepad", nullptr, nullptr);
#endif
#ifdef __linux__
    Display* display = XOpenDisplay(NULL);
    if (display == nullptr) {
        // Handle error: could not open display
        return;
    }
    Screen* screen = DefaultScreenOfDisplay(display);
    if (screen == nullptr) {
        // Handle error: could not get screen
        XCloseDisplay(display);
        return;
    }
    m_window = glfwCreateWindow(screen->width, screen->height,
                                "ImGui Notepad", nullptr, nullptr);
#endif

    if (!m_window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // VSync

#if defined(_WIN32)

    HWND hwnd = glfwGetWin32Window(m_window);
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
    SetWindowLong(hwnd, GWL_EXSTYLE, style | WS_EX_LAYERED);
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_ALPHA | LWA_COLORKEY);
    LoadIconWindow();

#elif defined(__APPLE__)

    id nswindow = glfwGetCocoaWindow(m_window);
    [nswindow setOpaque:NO];
    [nswindow setBackgroundColor:[NSColor clearColor]];

#elif defined(__linux__)

#if defined(GLFW_EXPOSE_NATIVE_X11)
    Display *display = glfwGetX11Display();
    Window x11Window = glfwGetX11Window(m_window);

    XSetWindowAttributes attrs;
    attrs.colormap = XCreateColormap(display, DefaultRootWindow(display),
                                     DefaultVisual(display, DefaultScreen(display)), AllocNone);
    attrs.border_pixel = 0;
    attrs.background_pixel = 0;

    XChangeWindowAttributes(display, x11Window, CWColormap | CWBorderPixel | CWBackPixel, &attrs);

    XMapWindow(display, x11Window);
    XFlush(display);
    LoadIconWindow();
#endif
    if (NFD_Init() != NFD_OKAY) {
        std::cerr << "Failed to init NFD: " << NFD_GetError() << std::endl;
    }
#endif
    
}

// OLD : Don`t use
void Window::SyncGlfwWithImGui(const ImVec2 &imguiPos, const ImVec2 &imguiSize)
{
#if defined(_WIN32)
    HWND hwnd = glfwGetWin32Window(m_window);
    MoveWindow(hwnd, (int)imguiPos.x, (int)imguiPos.y, (int)imguiSize.x, (int)imguiSize.y, TRUE);
#elif defined(__APPLE__)
    id nswindow = glfwGetCocoaWindow(m_window);
    NSRect frame = NSMakeRect(imguiPos.x, imguiPos.y, imguiSize.x, imguiSize.y);
    [nswindow setFrame:frame display:YES];
#elif defined(__linux__)
#if defined(GLFW_EXPOSE_NATIVE_X11)
    Display *display = glfwGetX11Display();
    Window x11Window = glfwGetX11Window(m_window);
    XMoveResizeWindow(display, x11Window, imguiPos.x, imguiPos.y, imguiSize.x, imguiSize.y);
    XFlush(display);
#else
    glfwSetWindowPos(m_window, imguiPos.x, imguiPos.y);
    glfwSetWindowSize(m_window, imguiSize.x, imguiSize.y);
#endif
#endif
}

void Window::InitImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.WantCaptureMouse = true;

    if (!m_fontPath.empty())
    {
        io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\Arial.ttf)", 20, nullptr,
                                     io.Fonts->GetGlyphRangesCyrillic());
    }
    else
    {
        io.Fonts->AddFontDefault();
    }

    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    const char *glsl_version = "#version 410";
    ImGui_ImplOpenGL3_Init(glsl_version);
    m_io = &io;
}

void Window::ToggleFullscreen() { m_fullscreen = !m_fullscreen; }

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
        ImVec2 centerPos((io.DisplaySize.x - windowSize.x) * 0.5f,
                         (io.DisplaySize.y - windowSize.y) * 0.5f);

        ImGui::SetNextWindowPos(centerPos, pos_cond);
        ImGui::SetNextWindowSize(windowSize, size_cond);
        windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar;
    }
}

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

GLFWwindow *Window::GetWindow() { return m_window; }
ImGuiIO &Window::GetImGuiIO() { return *m_io; }

void Window::setFontPath(const std::string &fontPath) { m_fontPath = fontPath; }
std::string Window::getFontPath() { return m_fontPath; }
bool Window::isFullscreen() { return m_fullscreen; }

void Window::LoadIconWindow()
{
    int iconWidth, iconHeight, channels;
    unsigned char *pixels = stbi_load(m_iconPath.c_str(), &iconWidth, &iconHeight, &channels, 4);
    if (!pixels)
    {
        std::cerr << "Failed to load window icon!" << std::endl;
        return;
    }

    if (pixels)
    {
        GLFWimage images[1];
        images[0].width = iconWidth;
        images[0].height = iconHeight;
        images[0].pixels = pixels;

        glfwSetWindowIcon(m_window, 1, images);

        stbi_image_free(pixels);
    }
    else
    {
        std::cerr << "Failed to load window icon!" << std::endl;
    }
}
}
