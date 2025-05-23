#ifndef FONT_H
#define FONT_H



#include <locale>
#include <string>


std::string GetFontPath();
void ShowFontWindow(char* path, bool& show_font_window, int& font_size);

#endif
