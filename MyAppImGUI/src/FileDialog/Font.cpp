#include "Font.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include <ShlObj.h>
#include <d3d11.h>// Include namespace for IFileOpenDialog>
#include <string>
#include "Window/Window.h"
#include "Editor/UIState.h"

void ShowFontWindow(std::string path, bool& show_font_window, int& font_size)
{
	ImGuiIO& io = ImGui::GetIO();
	const float dpi_scale = io.DisplayFramebufferScale.x;

	io.Fonts->TexDesiredWidth = static_cast<int>(2048 * dpi_scale);
	io.Fonts->Flags |= ImFontAtlasFlags_NoPowerOfTwoHeight;

	if (show_font_window)
	{
		if(ImGui::Begin("Font options", &show_font_window)){

		std::string pathFont;
		if (ImGui::Button("Set new font"))
		{
			pathFont = GetFontPath();
			path = pathFont;
		}

		ImGui::SameLine();
		ImGui::Text("Path of font: %s", (path.empty()) ? "None" : path);
		ImGui::Separator();
		ImGui::Text("Choose the font size:");

		ImGui::SliderInt("Font size", &font_size, 16, 32);
		ImGui::SameLine();

		if (ImGui::Button("Load font/Change font size"))
		{
			// io.Fonts->Clear(); // Clear existing fonts (make abort())

			ImFontConfig config;
			config.OversampleH = 3;
			config.SizePixels = static_cast<float>(font_size) * dpi_scale;

			ImFont* newFont = nullptr;

			if (path.empty())
			{
				newFont = io.Fonts->AddFontDefault(&config);
			}
			else
			{
				newFont = io.Fonts->AddFontFromFileTTF(path.c_str(), static_cast<float>(font_size) * dpi_scale, &config, io.Fonts->GetGlyphRangesCyrillic());
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
}

std::string GetFontPath()
{
    IFileOpenDialog* pFileOpen = nullptr;
    if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen))))
        return "Failed to open file dialog";

    COMDLG_FILTERSPEC fileTypes[] = {
        {L"TrueType Fonts (*.ttf)", L"*.ttf"},
    };

    pFileOpen->SetFileTypes(ARRAYSIZE(fileTypes), fileTypes);

    DWORD dwFlags;
    if (FAILED(pFileOpen->GetOptions(&dwFlags)) || FAILED(pFileOpen->SetOptions(dwFlags | FOS_FORCEFILESYSTEM)) || FAILED(pFileOpen->Show(nullptr)))
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