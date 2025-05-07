#ifndef RENDER_H
#define RENDER_H

#include <dxgi.h>

namespace ImGuiDirectX
{
	// To create a swap chain(For performance and fast draw window)
	inline int GetMonitorRefreshRate()
	{
		DEVMODE devMode = {};
		devMode.dmSize = sizeof(DEVMODE);

		// use EnumDisplaySettings for take options of current display
		if (EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &devMode))
		{
			return static_cast<int>(devMode.dmDisplayFrequency);
		}
		return 0; 
	}
	inline void Render(const HWND& hwnd, const float& x, const float& y)
	{
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		swapChainDesc.BufferDesc.Width = static_cast<UINT>(x);
		swapChainDesc.BufferDesc.Height = static_cast<UINT>(y);
		/*
		 * swapChainDesc.BufferDesc.RefreshRate.Numerator - means hz of monitor
		 */
		swapChainDesc.BufferDesc.RefreshRate.Numerator = GetMonitorRefreshRate();
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 1;

		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 3; 

		swapChainDesc.OutputWindow = hwnd;
		swapChainDesc.Windowed = TRUE; 
		//swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; 
	}
}
#endif