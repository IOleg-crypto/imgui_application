#ifndef FILEDIALOG_H
#define FILEDIALOG_H

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
#include <fstream>
#include <iostream>
#include <string>
#include <tchar.h>
// For save file 
#include <filesystem>


static HWND hwnd = nullptr;     // Global variable for window handle
static bool fullscreen = false; // Toggle for fullscreen mode

void SaveFileDialog(HWND hwnd, const std::string& CurrentTabInfo);
void ShowOpenFileDialog(HWND hwnd, std::string& tabContents, std::string &path);
void SaveFile(HWND hwnd , const std::string &path, const std::string& content);

#endif