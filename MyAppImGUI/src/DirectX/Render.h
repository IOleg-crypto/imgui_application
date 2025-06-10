#ifndef RENDER_H
#define RENDER_H

#include <dxgi.h>

namespace ImGuiDirectX
{
	/* Get the refresh rate of the monitor*/
	int GetMonitorRefreshRate();
	/* Render the window and have settings to improve performance*/
	void Render(const HWND& hwnd, const float& x, const float& y);
}
#endif