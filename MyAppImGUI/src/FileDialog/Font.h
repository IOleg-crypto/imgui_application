#ifndef FONT_H
#define FONT_H

#include <locale>
#include <string>

/*
  std::string GetFontPath(); - Additional function to get font path;
*/
std::string GetFontPath();
/*
*   void ShowFontWindow(char* path, bool& show_font_window, int& font_size); - Additional function to show font window;
*/
void ShowFontWindow(char* path, bool& show_font_window, int& font_size);

#endif
