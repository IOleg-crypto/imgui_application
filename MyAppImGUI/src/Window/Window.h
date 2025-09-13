#ifndef WINDOW_H  
#define WINDOW_H

#include "imgui.h"
#include "FileDialog/FileDialog.h"
#include "Editor/UIState.h"
#include "Font/Font.h"
#include "imgui_impl_glfw.h"

#include <string>

namespace ImGui {
class Window {
public:
    Window();
    ~Window();
public:
    void Init();                          // init window
    void ToggleFullscreen();              // toggle fullscreen
    void AboutWindow(bool& showDemoWindow); // about/info window
    void ApplyFullscreenLayout(ImGuiWindowFlags& windowFlags);
    void InitImGui();                     // init ImGui + font
    void setFontPath(const std::string& fontPath);
    GLFWwindow* GetWindow();
    // Old(don`t use)
    void SyncGlfwWithImGui(const ImVec2& imguiPos, const ImVec2& imguiSize);
    ImGuiIO& GetImGuiIO();
    std::string getFontPath();
    bool isFullscreen();
    void LoadIconWindow();
private:
    GLFWwindow* m_window;
    ImGuiIO* m_io;
    Font m_font; 
private:
    std::string m_iconPath;
    std::string m_fontPath;
    static bool m_fullscreen;
    bool prev_fullscreen;

public:
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar;
};
};

#endif // WINDOW_H
