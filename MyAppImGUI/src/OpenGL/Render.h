#ifndef RENDER_H
#define RENDER_H

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

/*
 * Returns the refresh rate of the primary monitor.
 */
int GetMonitorRefreshRate();

/*
 * Renders the ImGui window.
 *
 * Parameters:
 * - hwnd: handle to the window where rendering is done
 * - x, y: position offsets for rendering (useful for positioning UI)
 *
 * This function may include settings or optimizations to improve performance.
 */
void Render(GLFWwindow *window);

#endif // RENDER_H
