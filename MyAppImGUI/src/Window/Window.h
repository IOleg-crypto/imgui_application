#ifndef WINDOW_H 
#define WINDOW_H

#include <Windows.h>
#include <dxgi.h>
#include <string>
#include "imgui.h"
#include "FileDialog.h"
#include "Editor/UIState.h"

/**
 * @brief Encapsulates window creation and management for the ImGui Notepad app.
 *
 * Handles window class registration, icon loading, fullscreen toggle, message polling,
 * resize and occlusion events, and applying fullscreen layouts.
 */
class Window {
public:
    Window();

    /**
     * @brief Construct a new Window object with an existing window handle.
     * @param hwnd Handle to the created window.
     */
    Window(HWND& hwnd);

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
     * @brief Poll and handle window messages and events.
     * @param done Reference to a flag indicating if the app should quit.
     */
    void PollMessage(bool& done) const;

    /**
     * @brief Handle window resize event (e.g., reset swap chain buffers).
     * @param swapChain Pointer to the DXGI swap chain.
     */
    void HandleResize(IDXGISwapChain* swapChain) const;

    /**
     * @brief Handle occlusion (window visibility) events.
     * @param swapChainOccluded Reference to occlusion flag.
     * @param swapChain Pointer to the DXGI swap chain.
     */
    void HandleOcclusion(bool& swapChainOccluded, IDXGISwapChain* swapChain) const;

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
    HWND GetHWND();

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

private:
    HWND m_hwnd;                     ///< Handle to the window.
    HICON m_hIcon;                   ///< Window icon handle.
    WNDCLASSEXW m_wc{};              ///< Window class structure.
    ImGuiIO m_io;                    ///< ImGui IO object for configuration and input.

    std::wstring m_iconPath;         ///< Path to the icon file (wide string).
    std::string m_fontPath;          ///< Path to the font file.
    static bool m_fullscreen;        ///< Current fullscreen state.
    bool m_classRegistered = false;  ///< Flag indicating if the window class is registered.
    bool prev_fullscreen;            ///< Previous fullscreen state.

public:
    /// Default window flags: menu bar and horizontal scrollbar.
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar;
};

#endif // WINDOW_H
