#ifndef UISTATE_H
#define UISTATE_H

#include <string>
#include <imgui.h>
#include <vector>

struct UIState {
	bool showFontWindow = false;
	bool showInfoWindow = false;
	bool readOnly = false;
	bool themeChange = false;
	bool hideWindow = true;
	int selectedTab = 0;
	int fontSize = 16; // by default
	ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_AllowTabInput;
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar;
	std::string currentPath;
};

static UIState s_state;

#endif