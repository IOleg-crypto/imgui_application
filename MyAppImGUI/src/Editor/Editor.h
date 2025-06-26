#ifndef EDITOR_H
#define EDITOR_H

#include "imgui.h"
#include "Memory.h"
#include "UIState.h"
#include "FileDialog/FileDialog.h"
#include "Window/Window.h"

#include <string>
#include <vector>
#include <iostream>

class TabManager
{
private:
	std::vector<std::string>TabPages;
	std::vector<std::string>TabContent;
	std::string pathFile;
	std::string currentTabInfo;
	int selectedTab;
private:
	int pendingFontSize = 16;
	bool showWindow = false;
	bool shouldReloadFont = false;
	std::string pendingFontPath;
private:
	Window m_Window;
public:
	bool showFontWindow = false;
    std::string fontPath;
	int fontSize = 16;
	ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_AllowTabInput;
public:
	TabManager();
	~TabManager();

	void RenderMenuTab();
	void RenderInputTextField();
	void UpdateFontBeforeFrame();
	void ShowFontWindow();
	std::string GetFontPath();
	std::string GetCurrentInfo();
	std::string GetCurrentFilePath();

	bool &getShowWindow() { return showWindow; }

};
#endif
