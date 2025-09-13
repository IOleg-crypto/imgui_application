#include "Editor.h"
#include "Memory/Memory.h"
#include "Window/Window.h"
#include <stdlib.h>
#ifdef _WIN32
#include <ShlObj.h>
#include <d3d11.h>

#endif
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "nfd.h"
#include <filesystem>
#include <print>
#include <cmath>

TabManager::TabManager() : selectedTab(0), TabPages{"Page 1"}, TabContent{""}
{
    //@brief : All params init by default
}

TabManager::~TabManager() = default;

void TabManager::RenderMenuTab()
{
    bool earlyExit = false;

    if (ImGui::BeginMenuBar())
    {
        // ===== MENU =====
        if (ImGui::BeginMenu("Menu"))
        {
            // ReadOnly
            if (ImGui::MenuItem("ReadOnly", "Ctrl+M", &s_state.readOnly))
            {
                if (s_state.readOnly)
                    inputFlags |= ImGuiInputTextFlags_ReadOnly;
                else
                    inputFlags &= ~ImGuiInputTextFlags_ReadOnly;
            }

            // Save file
            if (ImGui::MenuItem("Save file", "Ctrl+F"))
            {
                if (pathFile.empty())
                {
                    pathFile = TabPages[selectedTab];
                    earlyExit = true;
                }
                SaveFile(pathFile, currentTabInfo);
                TabPages[selectedTab] = std::filesystem::path(pathFile).filename().string();
            }

            // Save file as
            if (ImGui::MenuItem("Save file as", "Ctrl+Shift+S"))
            {
                if (!pathFile.empty())
                {
                    TabPages[selectedTab] = std::filesystem::path(pathFile).filename().string();
                }
				SaveFileDialog(TabContent[selectedTab], pathFile);
            }

            // Open file
            if (ImGui::MenuItem("Open file", "Ctrl+O"))
            {
                ShowOpenFileDialog(currentTabInfo, pathFile);
                if (!TabPages.empty() && !pathFile.empty())
                {
                    TabPages[selectedTab] = std::filesystem::path(pathFile).filename().string();
                    memcpy(TabContent[selectedTab].data(), currentTabInfo.c_str(),
                           currentTabInfo.size() + 1);
                }
            }

            // Remove page
            if (ImGui::MenuItem("Remove page", "Delete"))
            {
                if (selectedTab >= 0 && selectedTab < static_cast<int>(TabPages.size()))
                {
                    TabPages.erase(TabPages.begin() + selectedTab);
                    TabContent.erase(TabContent.begin() + selectedTab);

                    if (selectedTab >= static_cast<int>(TabPages.size()))
                        selectedTab = static_cast<int>(TabPages.size() - 1);

                    if (TabPages.empty())
                    {
                        TabPages.emplace_back("Page1");
                        TabContent.emplace_back();
                    }
                }
            }

            // Fullscreen
            if (ImGui::MenuItem("Fullscreen", "3"))
            {
                m_Window.ToggleFullscreen();
            }

            // Exit
            if (ImGui::MenuItem("Exit", "Alt+F4"))
            {
                std::exit(0);
            }

            // Help
            if (ImGui::MenuItem("Help"))
            {
                s_state.showInfoWindow = !s_state.showInfoWindow;
            }

            ImGui::EndMenu();
        }

        // ===== FONT AND SIZE =====
        if (ImGui::BeginMenu("Font and size"))
        {
            if (ImGui::MenuItem("Font"))
                showFontWindow = true;

            ImGui::EndMenu();
        }

        // ===== THEME =====
        if (ImGui::BeginMenu("Theme"))
        {
            if (ImGui::MenuItem("Theme light/dark", "CTRL+R"))
            {
                s_state.themeChange = !s_state.themeChange;
                if (s_state.themeChange)
                    ImGui::StyleColorsLight();
                else
                    ImGui::StyleColorsDark();
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    // ===== INFO WINDOW =====
    if (s_state.showInfoWindow)
    {
        m_Window.AboutWindow(s_state.showInfoWindow);
    }

    if (earlyExit)
        return;
}

void TabManager::RenderInputTextField()
{

    if (ImGui::Button("+")) // Add a new tab
    {
        TabPages.emplace_back("Page" + std::to_string(TabPages.size() + 1));
        TabContent.emplace_back();

        selectedTab = static_cast<int>(TabPages.size()) - 1;

        constexpr size_t maxTabs = 45;
        if (TabPages.size() > maxTabs)
        {
            TabPages.pop_back();
            TabContent.pop_back();
        }
    }
    ImGui::SameLine();

    if (ImGui::BeginTabBar("MyTabBar"))
    {
        int tabToRemove = -1;
        float takeUpSpace = 220; // Space taken by the tab bar
        for (size_t i = 0; i < TabPages.size(); ++i)
        {
            bool open = true;
            if (ImGui::BeginTabItem(TabPages[i].c_str(), &open))
            {
                selectedTab = i;
                ImGui::Text("Content for %s", TabPages[i].c_str());
                // m_Window.GetImGuiIO().DisplaySize.x, m_Window.isFullscreen() ?
                // m_Window.GetImGuiIO().DisplaySize.y - takeUpSpace :
                // m_Window.GetImGuiIO().DisplaySize.y
                ImGui::InputTextMultiline("##InputText", TabContent[i].data(),
                                          TabContent[i].capacity(), ImVec2(999999, 999999),
                                          inputFlags | ImGuiInputTextFlags_CallbackResize,
                                          InputTextCallback, static_cast<void *>(&TabContent[i]));

                ImGui::EndTabItem();
            }

#ifdef _DEBUG
            static ImVec2 lastSize = ImVec2(0, 0);
            if (ImGui::IsWindowFocused())
            {
                ImVec2 currentSize = ImGui::GetWindowSize();
                if (currentSize.x != lastSize.x || currentSize.y != lastSize.y)
                {
                    system("cls"); // Is this the correct way to clear the console?
                    std::cout << std::format("Window resized: {} x {}\n", currentSize.x,
                                             currentSize.y);
                    lastSize = currentSize;
                }
            }
#endif

            if (!open)
            {
                tabToRemove = i;
            }
        }

        if (tabToRemove != -1)
        {
            if (TabPages.size() == 1)
            {
                TabPages[0] = "Page 1";
                TabContent[0].clear();
            }
            else
            {
                TabPages[tabToRemove] = std::move(TabPages.back());
                TabContent[tabToRemove] = std::move(TabContent.back());
                TabPages.pop_back();
                TabContent.pop_back();

                if (selectedTab >= static_cast<int>(TabPages.size()))
                    selectedTab = static_cast<int>(TabPages.size()) - 1;
            }
        }
    }
    // To get the current tab content and prevent memory leak
    if (!TabPages.empty() && (!TabContent[selectedTab].empty() || !TabPages[selectedTab].empty()))
    {
        currentTabInfo = TabContent[selectedTab];
    }

    ImGui::Separator();

    if (ImGui::Checkbox("Read Only", &s_state.readOnly))
    {
        if (s_state.readOnly)
        {
            inputFlags |= ImGuiInputTextFlags_ReadOnly;
        }
        else
        {
            inputFlags &= ~ImGuiInputTextFlags_ReadOnly;
        }
    }
    ImGui::EndTabBar();

    ImGuiIO &io = ImGui::GetIO();
    if (ImGui::IsKeyPressed(ImGuiKey_F) && io.KeyCtrl)
    {
        // SaveFile(m_Window.GetWindow(), pathFile, currentTabInfo);
    }

    if (ImGui::IsKeyPressed(ImGuiKey_S) && io.KeyCtrl && io.KeyShift)
    {
        // SaveFileDialog(m_Window.GetWindow(), currentTabInfo, pathFile);
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Delete))
    {
        if (TabPages.empty())
        {
            TabPages.emplace_back("Page " + std::to_string(TabPages.size() + 1));
            TabContent.emplace_back();
        }
        if (selectedTab >= 0 && selectedTab < static_cast<int>(TabPages.size()))
        {
            TabPages.erase(TabPages.begin() + selectedTab);
            TabContent.erase(TabContent.begin() + selectedTab);

            if (TabPages.empty())
            {
                TabPages.emplace_back("Page" + std::to_string(TabPages.size() + 1));
                TabContent.emplace_back();
            }
            selectedTab = std::min(selectedTab, static_cast<int>(TabPages.size()) - 1);
        }
    }

    if (ImGui::IsKeyPressed(ImGuiKey_3))
    {
        m_Window.ToggleFullscreen();
    }

    if (ImGui::IsKeyPressed(ImGuiKey_M) && io.KeyCtrl)
    {
        s_state.readOnly = !s_state.readOnly;
        inputFlags = s_state.readOnly ? (inputFlags | ImGuiInputTextFlags_ReadOnly)
                                      : (inputFlags & ~ImGuiInputTextFlags_ReadOnly);
    }

    if (ImGui::IsKeyPressed(ImGuiKey_F4) && io.KeyAlt)
    {
        std::exit(0);
    }

    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_F, false))
    {
        s_state.themeChange = !s_state.themeChange;
        if (s_state.themeChange)
        {
            ImGui::StyleColorsLight();
        }
        else
        {
            ImGui::StyleColorsDark();
        }
    }
}

void TabManager::ShowFontWindow()
{
    if (!showFontWindow)
        return;

    fontPath = m_Window.getFontPath();
    ImGui::Begin("Font options", &showFontWindow);

    // deleted second writing of font(Now in UpdateFontBeforeFrame)
    if (ImGui::Button("Set new font"))
    {
        std::string newPath = GetFontPath();
        if (!newPath.empty() && newPath != "Failed to get file path")
        {
            pendingFontPath = newPath;
            pendingFontSize = fontSize;
            shouldReloadFont = true;
        }
    }
    ImGui::SameLine();
    ImGui::Text("Path: %s", fontPath.empty() ? "None" : fontPath.c_str());
    ImGui::Separator();

    if (ImGui::SliderInt("Font size", &fontSize, 10, 32))
    {
        pendingFontSize = fontSize;
        ImGui::GetIO().FontGlobalScale = fontSize / 16.0f;
        shouldReloadFont = true;
    }

    ImGui::End();
}

void TabManager::UpdateFontBeforeFrame()
{
    if (!shouldReloadFont)
        return;

    ImGuiIO &io = ImGui::GetIO();
    float dpi_scale = io.DisplayFramebufferScale.x;

    io.Fonts->Clear();

    ImFontConfig cfg;
    cfg.OversampleH = 3;
    cfg.SizePixels = pendingFontSize * dpi_scale;

    if (pendingFontPath.empty())
    {
        pendingFontPath = m_Window.getFontPath();
    }

    ImFont *newFont = io.Fonts->AddFontFromFileTTF(pendingFontPath.c_str(), cfg.SizePixels, &cfg,
                                                   io.Fonts->GetGlyphRangesCyrillic());

    if (newFont)
    {
        io.FontDefault = newFont;
        fontPath = pendingFontPath;
        fontSize = pendingFontSize;
    }
    else
    {
        std::cerr << "Error: failed to load font at path: " << pendingFontPath << "\n";
    }

    shouldReloadFont = false;
}

std::string TabManager::GetFontPath()
{
    nfdu8char_t* outPath = nullptr;
    nfdu8filteritem_t filters[] = {{"TrueType Fonts", "ttf"}};
    nfdresult_t result = NFD_OpenDialogU8(&outPath, filters, 1, nullptr);

    std::string path;

    switch (result)
    {
        case NFD_OKAY:
            path = outPath;      
            NFD_FreePathU8(outPath);
            break;

        case NFD_CANCEL:
            break;

        default:
            std::cerr << "NFD error: " << NFD_GetError() << std::endl;
            break;
    }

    return path; // Return an empty string if no valid path was obtained
}

std::string TabManager::GetCurrentInfo() { return currentTabInfo; }

std::string TabManager::GetCurrentFilePath() { return pathFile; }