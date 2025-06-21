#ifndef EDITOR_H
#define EDITOR_H

#include "imgui.h"
#include "Memory.h"
#include "UIState.h"
#include "FileDialog/FileDialog.h"
#include "Window/Window.h"

#include <string>
#include <vector>

class TabManager
{
private:
	std::vector<std::string>TabPages;
	std::vector<std::string>TabContent;
	std::string pathFile;
	std::string currentTabInfo;
	int selectedTab;
private:
	Window m_Window;
	UIState s_state;
public:
	TabManager();
	~TabManager();

	void RenderMenuTab();
	void RenderInputTextField();

};
#endif
