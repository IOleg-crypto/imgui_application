#include "Font.h"

void ShowFontWindow(char* path, bool& show_font_window, int& font_size)
{
    ImGuiIO& io = ImGui::GetIO();
    float dpi_scale = io.DisplayFramebufferScale.x;

    io.Fonts->TexDesiredWidth = static_cast<int>(2048 * dpi_scale);
    io.Fonts->Flags |= ImFontAtlasFlags_NoPowerOfTwoHeight;

    if (show_font_window)
    {
        ImGui::Begin("Font options", &show_font_window);

        std::string pathFont;
        if (ImGui::Button("Set new font"))
        {
            pathFont = GetFontPath();
            strcpy(path, pathFont.c_str());
        }

        ImGui::SameLine();
        ImGui::Text("Path of font: %s", (strlen(path) == 0) ? "None" : path);
        ImGui::Separator();
        ImGui::Text("Choose the font size:");

        ImGui::SliderInt("Font size", &font_size, 16, 32);
        ImGui::SameLine();

        if (ImGui::Button("Load font/Change font size"))
        {
            // io.Fonts->Clear(); // Clear existing fonts (make abort())

            ImFontConfig config;
            config.OversampleH = 3;
            config.SizePixels = font_size * dpi_scale;

            ImFont* newFont = nullptr;

            if (strlen(path) == 0)
            {
                newFont = io.Fonts->AddFontDefault(&config);
            }
            else
            {
                newFont = io.Fonts->AddFontFromFileTTF(path, font_size * dpi_scale, &config, io.Fonts->GetGlyphRangesCyrillic());
            }

            if (newFont)
            {
                io.FontDefault = newFont;
                ImGui_ImplDX11_InvalidateDeviceObjects();
                ImGui_ImplDX11_CreateDeviceObjects();
            }
        }

        ImGui::End();
    }
}

std::string GetFontPath()
{
    IFileOpenDialog* pFileOpen = nullptr;
    if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen))))
        return "Failed to open file dialog";

    COMDLG_FILTERSPEC fileTypes[] = {
        {L"TrueType Fonts (*.ttf)", L"*.ttf"},
    };

    pFileOpen->SetFileTypes(ARRAYSIZE(fileTypes), fileTypes);

    DWORD dwFlags;
    if (FAILED(pFileOpen->GetOptions(&dwFlags)) || FAILED(pFileOpen->SetOptions(dwFlags | FOS_FORCEFILESYSTEM)) || FAILED(pFileOpen->Show(NULL)))
    {
        pFileOpen->Release();
        return "Failed to get file path";
    }

    IShellItem* pItem = nullptr;
    if (FAILED(pFileOpen->GetResult(&pItem)))
    {
        pFileOpen->Release();
        return "Failed to get file item";
    }

    PWSTR pszFilePath = nullptr;
    if (FAILED(pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath)))
    {
        pItem->Release();
        pFileOpen->Release();
        return "Failed to get file path";
    }

    // Convert to std::string
    std::wstring wideFilePath(pszFilePath);
    std::string filePath(wideFilePath.begin(), wideFilePath.end());
    return filePath;
}