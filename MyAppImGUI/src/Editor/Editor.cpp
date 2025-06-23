#include "Editor.h"
#include "Window/Window.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include <d3d11.h>
#include <ShlObj.h>
#include <filesystem>


TabManager::TabManager() : selectedTab(0), TabPages{ "Page 1" }, TabContent{ "" }
{
	//@brief : All params init by default
}

TabManager::~TabManager()
{
	TabPages.shrink_to_fit();
	TabContent.shrink_to_fit();
}

void TabManager::RenderMenuTab()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::MenuItem("ReadOnly", "Ctrl+M", &s_state.readOnly))
			{
				if (s_state.readOnly)
					s_state.inputFlags |= ImGuiInputTextFlags_ReadOnly;
				else
					s_state.inputFlags &= ~ImGuiInputTextFlags_ReadOnly;
			}
			if (ImGui::MenuItem("Save file as", "Ctrl+ Left Shift + S"))
			{
				SaveFileDialog(m_Window.GetHWND(), currentTabInfo, pathFile);

				if (!pathFile.empty())
				{
					// Only takes the file name
					std::filesystem::path filePath(pathFile);
					TabPages[selectedTab] = filePath.filename().string();
					TabContent[selectedTab].resize(currentTabInfo.size() + 1); // +1 for null terminator
					std::memcpy(TabContent[selectedTab].data(), currentTabInfo.c_str(), currentTabInfo.size() + 1);
				}
			}
			if (ImGui::MenuItem("Save file", "Ctrl+S"))
			{
				// To fix bug with empty name of tab
				if (pathFile.empty())
				{
					pathFile = TabPages[selectedTab];
					MessageBoxA(m_Window.GetHWND(), "No valid directory found", "File not saved", MB_OK);
				}
				else
				{
					SaveFile(m_Window.GetHWND(), pathFile, currentTabInfo);
					TabPages[selectedTab] = std::move(pathFile);
				}
			}
			if (ImGui::MenuItem("Open file", "Ctrl+O"))
			{
				ShowOpenFileDialog(m_Window.GetHWND(), currentTabInfo, pathFile);
				// To prevent add file, when tab don`t exist
				if (!TabPages.empty())
				{
					if (!pathFile.empty())
					{
						// Only takes the file name
						std::filesystem::path filePath(pathFile);
						TabPages[selectedTab] = filePath.filename().string();
						TabContent[selectedTab].resize(currentTabInfo.size() + 1); // +1 for null terminator
						std::memcpy(TabContent[selectedTab].data(), currentTabInfo.c_str(), currentTabInfo.size() + 1);
					}
				}
			}
			if (ImGui::MenuItem("Remove page", "Delete"))
			{
				if (selectedTab >= 0 && selectedTab <= static_cast<int>(TabPages.size())) // Ensure selectedTab is within valid range
				{
					TabPages.erase(TabPages.begin() + selectedTab);
					TabContent.erase(TabContent.begin() + selectedTab);

					// Optionally, update the selectedTab index to a valid one after deletion
					if (selectedTab <= static_cast<int>(TabPages.size()))
					{
						selectedTab = static_cast<int>(TabPages.size() - 1); // Move to the last tab if the deleted tab was the last one
					}
					// Added fix
					if (TabPages.empty())
					{
						TabPages.emplace_back("Page" + std::to_string(TabPages.size() + 1));
						TabContent.emplace_back();
					}
				}
			}
			if (ImGui::MenuItem("Fullscreen", "3"))
			{
				//ToggleFullscreen();
			}
			if (ImGui::MenuItem("Exit", "Alt+F4"))
			{
				::PostQuitMessage(0);
			}
			if (ImGui::MenuItem("Help"))
			{
				s_state.showInfoWindow = !s_state.showInfoWindow;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Font and size"))
		{
			if (ImGui::MenuItem("Font"))
			{
				showFontWindow = true;
			}
			ImGui::EndMenu();
		}
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

	if (s_state.showInfoWindow)
	{
		m_Window.AboutWindow(s_state.showInfoWindow);
	}
}

void TabManager::RenderInputTextField()
{
	if (ImGui::Button("Add page"))
	{
		TabPages.emplace_back("Page" + std::to_string(TabPages.size() + 1));
		TabContent.emplace_back();

		selectedTab = static_cast<int>(TabPages.size()) - 1;

		constexpr size_t maxTabs = 45;
		if (TabPages.size() > maxTabs)
		{
			TabPages.pop_back();
		}
	}

	ImGui::Separator();
	ImGui::BeginTabBar("MyTabBar");

	for (int i = 0; i < static_cast<int>(TabPages.size()); ++i)
	{
		bool open = true;
		if (ImGui::BeginTabItem(TabPages[i].c_str(), &open))
		{
			selectedTab = i;
			ImGui::Text("Content for %s", TabPages[i].c_str());

			ImGui::InputTextMultiline("##InputText", TabContent[i].data(), TabContent[i].capacity(),
				ImVec2(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)), s_state.inputFlags | ImGuiInputTextFlags_CallbackResize, InputTextCallback, static_cast<void*>(&TabContent[i]));

			ImGui::EndTabItem();
		}

		// If tab is closed, remove it (using reverse iteration to avoid shifting)
		if (!open && i < static_cast<int>(TabPages.size()))
		{
			TabPages[i] = std::move(TabPages.back()); // Move last item into current position
			TabContent[i] = std::move(TabContent.back());
			// O(1) instead of O(n)
			TabPages.pop_back();
			TabContent.pop_back();
			TabPages.shrink_to_fit();
			TabContent.shrink_to_fit();
			if ((selectedTab >= static_cast<int>(TabPages.size())))
			{
				selectedTab = static_cast<int>(TabPages.size() - 1);
			}
			// To avoid situation when tabTitles empty(user delete all tabs)
			if (TabPages.empty())
			{
				TabPages.emplace_back("Page" + std::to_string(TabPages.size() + 1));
				TabContent.emplace_back();
			}
		}
	}

	ImGui::Separator();

	if (ImGui::Checkbox("Read Only", &s_state.readOnly))
	{
		if (s_state.readOnly)
			s_state.inputFlags |= ImGuiInputTextFlags_ReadOnly;
		else
			s_state.inputFlags &= ~ImGuiInputTextFlags_ReadOnly;
	}

	ImGui::EndTabBar();

}

void TabManager::ShowFontWindow() {
	if (!showFontWindow)
		return;

	fontPath = m_Window.getFontPath();
	ImGui::Begin("Font options", &showFontWindow);

	// deleted second writing of font(Now in UpdateFontBeforeFrame)
	if (ImGui::Button("Set new font")) {
		std::string newPath = GetFontPath();
		if (!newPath.empty()) {
			pendingFontPath = newPath;
			pendingFontSize = fontSize;
			shouldReloadFont = true;
		}
	}
	ImGui::SameLine();
	ImGui::Text("Path: %s", fontPath.empty() ? "None" : fontPath.c_str());
	ImGui::Separator();

	if (ImGui::SliderInt("Font size", &fontSize, 10, 32)) {
		pendingFontSize = fontSize;
		ImGui::GetIO().FontGlobalScale = fontSize / 16.0f;	
		shouldReloadFont = true;
	}

	ImGui::End();
}

void TabManager::UpdateFontBeforeFrame() {
	if (!shouldReloadFont)
		return;

	ImGuiIO& io = ImGui::GetIO();
	float dpi_scale = io.DisplayFramebufferScale.x;

	io.Fonts->Clear();

	ImFontConfig cfg;
	cfg.OversampleH = 3;
	cfg.SizePixels = pendingFontSize * dpi_scale;
	

	ImFont* newFont = nullptr;
	if (pendingFontPath.empty())
		newFont = io.Fonts->AddFontFromFileTTF(m_Window.getFontPath().c_str(),
			16.0f * dpi_scale,
			&cfg,
			io.Fonts->GetGlyphRangesCyrillic());
	else
		newFont = io.Fonts->AddFontFromFileTTF(
			pendingFontPath.c_str(),
			cfg.SizePixels,
			&cfg,
			io.Fonts->GetGlyphRangesCyrillic()
		);

	if (newFont)
		io.FontDefault = newFont;

	ImGui_ImplDX11_InvalidateDeviceObjects();
	ImGui_ImplDX11_CreateDeviceObjects();

	fontPath = pendingFontPath;
	fontSize = pendingFontSize;
	shouldReloadFont = false;
}

std::string TabManager::GetFontPath()
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