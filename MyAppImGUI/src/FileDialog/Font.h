#ifndef FONT_H
#define FONT_H

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include <ShlObj.h>
#include <Shlwapi.h>
#include <commdlg.h> // Include Windows common dialogs header
#include <d3d11.h>
#include <shobjidl_core.h> // Include namespace for IFileOpenDialog>

// Include default C++ libraries
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <locale>
#include <string>
#include <tchar.h>
#include <vector>

std::string GetFontPath();
void ShowFontWindow(char* path, bool& show_font_window, int& font_size);

#endif
