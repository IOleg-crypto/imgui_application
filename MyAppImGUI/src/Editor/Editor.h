#ifndef EDITOR_H
#define EDITOR_H

#include <string>
#include <vector>
#include <imgui.h>
#include "FileDialog/FileDialog.h"

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
public:
	TabManager();
	~TabManager();
};
#endif
