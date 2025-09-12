#ifndef EDITOR_H 
#define EDITOR_H

#include "imgui.h"
#include "UIState.h"
#include "FileDialog/FileDialog.h"
#include "Window/Window.h"


#include <string>
#include <vector>
#include <iostream>


/**
 * @brief Manages multiple text editing tabs within the ImGui Notepad application.
 *
 * Responsible for rendering tabs, handling font changes, tracking file paths and content,
 * and interacting with the main application window.
 */
class TabManager
{
private:
    std::vector<std::string> TabPages;      ///< Stores the names (titles) of open tabs.
    std::vector<std::string> TabContent;    ///< Stores the text content of each tab.
    std::string pathFile;                   ///< Path of the currently open file.
    std::string currentTabInfo;             ///< Information about the current tab (e.g., filename or metadata).
    int selectedTab;                        ///< Index of the currently selected tab.

private:
    int pendingFontSize = 16;               ///< Font size pending to be applied.
    bool showWindow = false;                ///< Visibility flag for some modal/pop-up windows.
    bool shouldReloadFont = false;          ///< Flag indicating whether the font should be reloaded.
    std::string pendingFontPath;            ///< Path to a font that is pending to be loaded.

private:
    ImGui::Window m_Window;                        ///< Main application window object.

public:
    bool showFontWindow = false;            ///< Visibility flag for the font settings window.
    std::string fontPath;                   ///< Path to the currently used font.
    int fontSize = 16;                      ///< Currently applied font size.
    ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_NoHorizontalScroll; ///< ImGui input flags (e.g., tab handling).

public:
    /**
     * @brief Construct a new TabManager object.
     */
    TabManager();

    /**
     * @brief Destroy the TabManager object.
     */
    ~TabManager();

    /**
     * @brief Render the menu tab bar (e.g., File, Edit, Font options).
     */
    void RenderMenuTab();

    /**
     * @brief Render the input text field for editing content within the selected tab.
     */
    void RenderInputTextField();

    /**
     * @brief Apply any pending font path/size changes before the ImGui frame starts.
     */
    void UpdateFontBeforeFrame();

    /**
     * @brief Show the font selection and size adjustment window.
     */
    void ShowFontWindow();

    /**
     * @brief Get the currently active font path (used in ImGui).
     * @return Path to the font file.
     */
    std::string GetFontPath();

    /**
     * @brief Get metadata or status info of the current tab.
     * @return String with current tab information.
     */
    std::string GetCurrentInfo();

    /**
     * @brief Get the file path of the currently active tab.
     * @return String containing the file path.
     */
    std::string GetCurrentFilePath();

    /**
     * @brief Get a reference to the showWindow flag (used for toggling visibility).
     * @return Reference to a bool that controls visibility.
     */
    bool& getShowWindow() { return showWindow; }
    int ImGuiInputTextWordWrap(ImGuiInputTextCallbackData* data);
    int CombinedInputTextCallback(ImGuiInputTextCallbackData* data);
	
};


#endif // EDITOR_H
