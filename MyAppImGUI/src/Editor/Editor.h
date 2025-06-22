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
	bool showWindow = false;
private:
	std::string pendingFontPath;
	int pendingFontSize = 16;
	bool shouldReloadFont = false;
private:
	Window m_Window;
public:
	bool showFontWindow = false;
    std::string fontPath;
	int fontSize = 16;
public:
	TabManager();
	~TabManager();

	void RenderMenuTab();
	void RenderInputTextField();
	void UpdateFontBeforeFrame();
	void ShowFontWindow();
	std::string GetFontPath();

	bool &getShowWindow() { return showWindow; }

};
#endif
