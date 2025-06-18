#ifndef EDITOR_H
#define EDITOR_H

#include <string>
#include <vector>
#include <imgui.h>

namespace ImGuiNotepad {
	struct EditorSettings {
		bool show_font_window = false;
		bool read_only = false;
		ImGuiInputTextFlags input_flags = ImGuiInputTextFlags_AllowTabInput;
		ImGuiWindowFlags window_flags =
			ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar;
		int font_size = 16;
	};

	struct TabManager {
		int selectedTab = 0;
		std::vector<std::string> tabTitles = { "Page1" };
		std::vector<std::string> tabContents = { "" };
		std::string pathFile;
		std::string currentTabInfo;
	};

	struct WindowState {
		bool show_info_window = false;
		bool theme_change = false;
		bool hide_window = true;
		bool fullscreen = false;
		bool show_another_window = false;
	};
}
#endif
