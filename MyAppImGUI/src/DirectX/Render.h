#ifndef RENDER_H 
#define RENDER_H

#include <dxgi.h> // For DXGI interfaces (used to query display info like refresh rate)

namespace ImGuiDirectX
{
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
	void Render(const HWND& hwnd);
}

#endif // RENDER_H
