#include "Editor.h"


TabManager::TabManager() : pathFile(""), selectedTab(0) , TabPages{"Page 1"} , TabContent{}
{

}

TabManager::~TabManager()
{
	TabPages.shrink_to_fit();
	TabContent.shrink_to_fit();
}
