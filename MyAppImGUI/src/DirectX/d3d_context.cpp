#include "d3d_context.h"

#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "winuser.h"
#include <windows.h>


// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);

    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != nullptr && wParam != SIZE_MINIMIZED)
        {
            g_ResizeWidth = static_cast<UINT>(LOWORD(lParam));
            g_ResizeHeight = static_cast<UINT>(HIWORD(lParam));
        }
        break;

    case WM_NCHITTEST:
        {
            LRESULT hit = DefWindowProc(hWnd, msg, wParam, lParam);

            // Allow window dragging only when ImGui is NOT hovered
            if (hit == HTCLIENT && !ImGui::IsAnyItemHovered() && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
            {
                return HTCAPTION; // Allow dragging in empty areas
            }

            return hit;
        }

    case WM_MOUSEACTIVATE:
        return MA_ACTIVATE; // Ensures mouse clicks activate the window

    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;

    case WM_ENTERSIZEMOVE:
        g_is_resizing_or_moving = true;
        break;
    case WM_EXITSIZEMOVE:
        g_is_resizing_or_moving = false;
        break;
    default:;
    }

    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}