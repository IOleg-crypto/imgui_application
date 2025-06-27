#ifndef UISTATE_H 
#define UISTATE_H

#include <string>
#include <imgui.h>
#include <vector>

/**
 * @brief Represents the global UI state flags used across the application.
 *
 * Tracks visibility and behavior options such as theme changes, read-only mode,
 * and visibility of specific UI windows.
 */
struct UIState {
    bool showFontWindow = false;   ///< Flag to show/hide the font selection window.
    bool showInfoWindow = false;   ///< Flag to show/hide the application info window.
    bool readOnly = false;         ///< If true, text fields are in read-only mode.
    bool themeChange = false;      ///< If true, triggers a theme/style update.
    bool hideWindow = true;        ///< If true, certain UI elements/windows are hidden.
};

/**
 * @brief Global instance of the UI state, accessible throughout the application.
 */
static UIState s_state;

#endif // UISTATE_H