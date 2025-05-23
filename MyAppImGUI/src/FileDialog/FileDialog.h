#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <d3d11.h>// Include namespace for IFileOpenDialog>

// Include default C++ libraries
#include <string>
#include <filesystem>


static HWND hwnd = nullptr;     // Global variable for window handle
static bool fullscreen = false; // Toggle for fullscreen mode

void SaveFileDialog(const HWND &hwnd, const std::string& CurrentTabInfo ,std::string& path);
void ShowOpenFileDialog(const HWND &hwnd, std::string& tabContents, std::string& pathFile);
void SaveFile(const HWND &hwnd , const std::string &path, const std::string& content);

#endif