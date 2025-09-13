#include "Font.h"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#endif

namespace fs = std::filesystem;

Font::Font()
{
#ifdef _WIN32
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(nullptr, CSIDL_FONTS, nullptr, 0, path))) {
        m_windowsFontFolder = path;
    } else {
        m_windowsFontFolder = "C:\\Windows\\Fonts"; 
    }
#else
    m_linuxFontFolder = "/usr/share/fonts";
#endif
}

Font::~Font()
{
}

void Font::CheckFontInSystemDirectory(const std::string& fontDirectory)
{
    if (!fs::exists(fontDirectory)) {
        std::cerr << "Directory does not exist: " << fontDirectory << std::endl;
        return;
    }

    for (const auto& entry : fs::recursive_directory_iterator(fontDirectory)) {
        if (entry.is_regular_file()) {
            auto ext = entry.path().extension().string();
            if (ext == ".ttf" || ext == ".otf") {
                std::cout << "Found font: " << entry.path().string() << std::endl;
            }
        }
    }
}

std::string Font::GetFontWindowsFolder() const
{
    return m_windowsFontFolder;
}

std::string Font::GetFontLinuxFolder() const
{
    return m_linuxFontFolder;
}
