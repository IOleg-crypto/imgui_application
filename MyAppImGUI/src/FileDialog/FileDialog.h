#ifndef FILEDIALOG_H 
#define FILEDIALOG_H

#include <d3d11.h> // Includes DirectX 11 header (also brings in definitions like IFileOpenDialog)

// Include standard C++ libraries
#include <string>       // For using std::string
#include <filesystem>   // For handling file system paths (C++17 and later)

// Global variable for window handle (HWND)
// Note: 'static' here means each translation unit (.cpp file) that includes this header gets its own copy
static HWND hwnd = nullptr;

// Opens a Save File dialog and writes 'CurrentTabInfo' to selected path
// 'path' is updated with the selected file path
void SaveFileDialog(const HWND& hwnd, const std::string& CurrentTabInfo, std::string& path);

// Opens an Open File dialog and loads the content of the selected file into 'tabContents'
// 'pathFile' is updated with the selected file path
void ShowOpenFileDialog(const HWND& hwnd, std::string& tabContents, std::string& pathFile);

// Saves the given 'content' to the specified 'path'
void SaveFile(const HWND& hwnd, const std::string& path, const std::string& content);

#endif // FILEDIALOG_H
