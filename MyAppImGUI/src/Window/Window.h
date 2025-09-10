#ifndef WINDOW_H 
#define WINDOW_H

#include <string>
#include "imgui.h"
#include "FileDialog/FileDialog.h"
#include "Editor/UIState.h"
#include "imgui_impl_glfw.h"

/**
 * @brief Encapsulates window creation and management for the ImGui Notepad app.
 *
 * Handles window class registration, icon loading, fullscreen toggle, message polling,
 * resize and occlusion events, and applying fullscreen layouts.
 */
class Window {
public:
    Window();
    ~Window();

    /**
     * @brief Initialize the window class and load the icon.
     * Must be called before registering the window class or creating the window.
     */
    void Init();

    /**
     * @brief Toggles fullscreen mode on or off.
     */
    void ToggleFullscreen();

    /**
     * @brief Display the About window showing app info and performance stats.
     * @param showDemoWindow Reference to a flag controlling visibility.
     * @param io ImGuiIO structure providing frame timing information.
     */
    void AboutWindow(bool& showDemoWindow);

    /**
     * @brief Apply fullscreen layout settings for the ImGui window.
     * @param windowFlags Reference to ImGuiWindowFlags to modify.
     */
    void ApplyFullscreenLayout(ImGuiWindowFlags& windowFlags);

    /**
     * @brief Initialize ImGui context and load the font file.
     */
    void InitImGui();

    /**
     * @brief Set the font file path used by ImGui.
     * @param fontPath Path to the TTF/OTF font file.
     */
    void setFontPath(const std::string& fontPath);

    /**
     * @brief Get the native window handle.
     * @return HWND Handle to the window.
     */
    GLFWwindow* GetWindow();

    void SyncGlfwWithImGui(const ImVec2 &imguiPos, const ImVec2 &imguiSize);

    /**
     * @brief Get the ImGui IO structure.
     * @return Reference to ImGuiIO.
     */
    ImGuiIO& GetImGuiIO();

    /**
     * @brief Get the current font file path.
     * @return Path to the font file.
     */
    std::string getFontPath();

    /**
     * @brief Check if the window is currently in fullscreen mode.
     * @return true if fullscreen, false otherwise.
     */
    bool isFullscreen();
    // Loading icon from resource
    void LoadIconWindow();
private:
    GLFWwindow *m_window;                     ///< Handle to the window.
    ImGuiIO *m_io;                    ///< ImGui IO object for configuration and input.
    std::string m_iconPath;         ///< Path to the icon file (wide string).
    std::string m_fontPath;          ///< Path to the font file.
    static bool m_fullscreen;        ///< Current fullscreen state.
    bool prev_fullscreen;            ///< Previous fullscreen state.

public:
    /// Default window flags: menu bar and horizontal scrollbar.
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar;
};

#endif // WINDOW_H
