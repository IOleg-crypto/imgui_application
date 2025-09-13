#ifndef FONT_H
#define FONT_H

#include <filesystem>
#include <string>

class Font
{
private:
    std::string m_windowsFontFolder;
    std::string m_linuxFontFolder;

public:
    Font();
    ~Font();

public:
    std::string GetFontWindowsFolder() const;
    std::string GetFontLinuxFolder() const;
    void CheckFontInSystemDirectory(const std::string &fontDirectory);
};

#endif // FONT_H
