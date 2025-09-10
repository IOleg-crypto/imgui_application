#include "Editor.h"
#include "Memory/Memory.h"
#include "Window/Window.h"
#include <d3d11.h>
#include <ShlObj.h>
#include <filesystem>
#include <print>

#include "imgui.h"
#include "imgui_impl_opengl3.h"


TabManager::TabManager() : selectedTab(0), TabPages{ "Page 1" }, TabContent{ "" }
{
	//@brief : All params init by default
}

TabManager::~TabManager() = default;

void TabManager::RenderMenuTab()
{
	bool earlyExit = false; // To exit the loop
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::MenuItem("ReadOnly", "Ctrl+M", &s_state.readOnly))
			{
				if (s_state.readOnly)
					inputFlags |= ImGuiInputTextFlags_ReadOnly;
				else
					inputFlags &= ~ImGuiInputTextFlags_ReadOnly;
			}
			if (ImGui::MenuItem("Save file", "Ctrl+F"))
			{
				if (pathFile.empty())
				{
					pathFile = TabPages[selectedTab];
					// MessageBoxA(m_Window.GetHWND(), "No valid directory found", "File not saved", MB_OK);
					earlyExit = true;
				}

				SaveFile(pathFile, currentTabInfo);
				TabPages[selectedTab] = std::filesystem::path(pathFile).filename().string();
			}

			if (ImGui::MenuItem("Save file as", "Ctrl+Shift+S"))
			{
				SaveFileDialog(currentTabInfo, pathFile);
				if (!pathFile.empty()) {
					TabPages[selectedTab] = std::filesystem::path(pathFile).filename().string();
				}
			}
			if (ImGui::MenuItem("Open file", "Ctrl+O"))
			{
				ShowOpenFileDialog(currentTabInfo, pathFile);
				// To prevent add file, when tab don`t exist
				if (!TabPages.empty())
				{
					if (!pathFile.empty())
					{
						// Only takes the file name
						std::filesystem::path filePath(pathFile);
						TabPages[selectedTab] = filePath.filename().string();
						//TabContent[selectedTab].resize(currentTabInfo.size() + 1); // +1 for null terminator
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
				m_Window.ToggleFullscreen();				
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
		
	}
	ImGui::EndMenuBar();

	if (s_state.showInfoWindow)
	{
		m_Window.AboutWindow(s_state.showInfoWindow);
	}

	if (earlyExit)
	{
		return;
	}
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
                 //m_Window.GetImGuiIO().DisplaySize.x, m_Window.isFullscreen() ? m_Window.GetImGuiIO().DisplaySize.y - takeUpSpace : m_Window.GetImGuiIO().DisplaySize.y
				ImGui::InputTextMultiline("##InputText", TabContent[i].data(), TabContent[i].capacity(),
					ImVec2(999999 , 999999),
					inputFlags | ImGuiInputTextFlags_CallbackResize,
					InputTextCallback,
					static_cast<void*>(&TabContent[i]));

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
					std::cout << std::format("Window resized: {} x {}\n", currentSize.x, currentSize.y);
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
				TabPages[0] = "Page1";
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
		if (s_state.readOnly) {
			inputFlags |= ImGuiInputTextFlags_ReadOnly;
		}
		else {
			inputFlags &= ~ImGuiInputTextFlags_ReadOnly;
		}
	}
	ImGui::EndTabBar();

	ImGuiIO& io = ImGui::GetIO();
	if (ImGui::IsKeyPressed(ImGuiKey_F) && io.KeyCtrl) {
		SaveFile(pathFile, currentTabInfo);
	}

	if (ImGui::IsKeyPressed(ImGuiKey_S) && io.KeyCtrl && io.KeyShift) {
		SaveFileDialog(currentTabInfo, pathFile);
	}

	if (ImGui::IsKeyPressed(ImGuiKey_Delete)) {
		if (TabPages.empty())
		{
			TabPages.emplace_back("Page " + std::to_string(TabPages.size() + 1));
			TabContent.emplace_back();
		}
		if (selectedTab >= 0 && selectedTab < static_cast<int>(TabPages.size())) {
			TabPages.erase(TabPages.begin() + selectedTab);
			TabContent.erase(TabContent.begin() + selectedTab);

			if (TabPages.empty()) {
				TabPages.emplace_back("Page" + std::to_string(TabPages.size() + 1));
				TabContent.emplace_back();
			}
			selectedTab = min(selectedTab, static_cast<int>(TabPages.size()) - 1);
		}
	}

	if (ImGui::IsKeyPressed(ImGuiKey_3)) {
		m_Window.ToggleFullscreen();
	}

	if (ImGui::IsKeyPressed(ImGuiKey_M) && io.KeyCtrl) {
		s_state.readOnly = !s_state.readOnly;
		inputFlags = s_state.readOnly ? (inputFlags | ImGuiInputTextFlags_ReadOnly)
			: (inputFlags & ~ImGuiInputTextFlags_ReadOnly);
	}

	if (ImGui::IsKeyPressed(ImGuiKey_F4) && io.KeyAlt) {
		::PostQuitMessage(0);
	}

	if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_F, false))
	{
		s_state.themeChange = !s_state.themeChange;
		if (s_state.themeChange) {
			ImGui::StyleColorsLight();
		}
		else {
			ImGui::StyleColorsDark();
		}
	}
}

void TabManager::ShowFontWindow() {
	if (!showFontWindow)
		return;

	fontPath = m_Window.getFontPath();
	ImGui::Begin("Font options", &showFontWindow);

	// deleted second writing of font(Now in UpdateFontBeforeFrame)
	if (ImGui::Button("Set new font")) {
		std::string newPath = GetFontPath();
		if (!newPath.empty() && newPath != "Failed to get file path") {
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

	if (pendingFontPath.empty()) {
		pendingFontPath = m_Window.getFontPath();
	}

	ImFont* newFont = io.Fonts->AddFontFromFileTTF(
		pendingFontPath.c_str(),
		cfg.SizePixels,
		&cfg,
		io.Fonts->GetGlyphRangesCyrillic()
	);

	if (newFont) {
		io.FontDefault = newFont;
		fontPath = pendingFontPath;
		fontSize = pendingFontSize;
	}
	else {
		std::cerr << "Error: failed to load font at path: " << pendingFontPath << "\n";
	}

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
	return std::string(wideFilePath.begin(), wideFilePath.end());
}

std::string TabManager::GetCurrentInfo()
{
	return currentTabInfo;
}

std::string TabManager::GetCurrentFilePath()
{
	return pathFile;
}



