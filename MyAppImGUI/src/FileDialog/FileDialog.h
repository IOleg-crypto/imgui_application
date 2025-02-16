#pragma once

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

static HWND hwnd = nullptr;     // Global variable for window handle
static bool fullscreen = false; // Toggle for fullscreen mode


void SaveFileDialog(HWND hwnd, std::string& CurrentTabInfo)
{
    OPENFILENAME ofn;
    TCHAR szFile[MAX_PATH] = _T(""); // Buffer to store the selected file name

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = _T("Text Files (*.txt)\0*.txt\0Binary Files (*.bin)\0*.bin\0All Files (*.*)\0*.*\0");
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
    ofn.lpstrDefExt = _T("txt");

    if (GetSaveFileName(&ofn))
    {
        bool isBinary = (_tcsstr(szFile, _T(".bin")) != NULL);

        if (isBinary)
        {
            std::ofstream outFile(szFile, std::ios::out | std::ios::binary);
            if (outFile)
            {
                size_t length = CurrentTabInfo.length();
                outFile.write(reinterpret_cast<const char*>(CurrentTabInfo.c_str()), length);
                outFile.close();
            }
        }
        else
        {
            // Save as a text file
            std::ofstream outFile(szFile);
            if (outFile)
            {
                for (const auto& content : CurrentTabInfo)
                {
                    outFile << content << std::endl;
                }
                outFile.close();
            }
        }
    }
    // Fix to show message
    if (szFile != NULL)
    {
        MessageBox(hwnd, szFile, _T("File Saved"), MB_OK);
    }
}

void ShowOpenFileDialog(HWND hwnd, std::string& tabContents)
{
    OPENFILENAME ofn;                // Structure for the file dialog
    TCHAR szFile[MAX_PATH] = _T(""); // Buffer to store the selected file name

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = _T("Text Files (*.txt)\0*.txt\0Binary Files (*.bin)\0*.bin\0All Files (*.*)\0*.*\0");
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    ofn.lpstrDefExt = _T("txt"); // Default extension is .txt

    if (GetOpenFileName(&ofn))
    {
        bool isBinary = (_tcsstr(szFile, _T(".bin")) != NULL);

        if (isBinary)
        {
            // Open and read binary file
            std::ifstream inFile(szFile, std::ios::in | std::ios::binary);
            if (inFile)
            {
                inFile.read(reinterpret_cast<char*>(tabContents.data()), tabContents.size());
                inFile.close();
            }
        }
        else
        {
            // Open and read text file
            std::ifstream inFile(szFile);
            if (inFile)
            {
                std::string line;
                while (std::getline(inFile, line))
                {
                    tabContents += line + "\n";
                }
                inFile.close();
            }
        }
    }

    if (szFile != NULL)
    {
        MessageBox(hwnd, szFile, _T("File opened"), MB_OK);
    }
}
