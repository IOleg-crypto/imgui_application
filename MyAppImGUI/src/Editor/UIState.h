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
};

static UIState s_state;

#endif