#ifndef FONT_H 
#define FONT_H

#include <locale>  // For locale-specific operations (if needed)
#include <string>  // For using std::string
#include "Editor/UIState.h"
#include <iostream>

/*
 * Returns the path to the desired font file.
 * Can be used for loading custom fonts dynamically.
 */
std::string GetFontPath();

/*
 * Displays a font selection window.
 *
 * Parameters:
 * - path: the character buffer to store the selected font path
 * - show_font_window: a flag to toggle the visibility of the font window
 * - font_size: reference to the current font size to allow adjustment
 */
void ShowFontWindow(std::string path, bool& show_font_window, int& font_size);

#endif // FONT_H
