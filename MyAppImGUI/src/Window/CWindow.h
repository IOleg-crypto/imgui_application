#ifndef CWINDOW_H
#define CWINDOW_H

#include "FileDialog.h"
#include "imgui.h"

namespace ImGuiNotepad {
    class CWindow {
    private:
        float m_x;
        float m_y;
        HICON &m_hIcon;
        HWND &m_hwnd;
        WNDCLASSEXW &m_wc;
        const wchar_t *m_iconPath = L"../assets/icon/icon.ico";
    public:
        CWindow(WNDCLASSEXW &wc , HICON &hIcon , HWND &hwnd, const float &x , float &y);
        /*
          Explain : I don`t need that , this CWindow init window.
          NOT COPY AND NOT MOVE!!!!
        */
        CWindow(const CWindow&other) = delete;
        CWindow(CWindow &&other) = delete;
    public:
        /*
         * Initializes the window class and icon.
         * This function sets up the WNDCLASSEXW structure and loads the icon from file.
         * Should be called before registering the window class or creating the window.
         */
        void Init();
        /*
         * Function to toggle fullscreen mode.
         * Flips the'fullscreen' flag on or off.
         */
        static void ToggleFullscreen(bool &fullscreen);
        /*
         * Function to show the About window (info about the application).
         * Displays application name and performance statistics (ms/frame and FPS).
         *
         * Parameters:
         * - show_demo_window: reference to a flag controlling window visibility
         * - io: ImGuiIO structure providing frame timing info
         */
        static void AboutWindow(bool& show_demo_window, const ImGuiIO& io);
    };
}
#endif //CWINDOW_H
