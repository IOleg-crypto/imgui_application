#include "d3d_context.h"

#include "imgui_impl_win32.h"
#include "windowsx.h"
#include "winuser.h"
#include <Windows.h>
#include <iostream>

// Direct3D device pointer
ID3D11Device* g_pd3dDevice = nullptr;
// Direct3D device context pointer
ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
// Swap chain for presenting rendered frames
IDXGISwapChain* g_pSwapChain = nullptr;

// Flag indicating if the swap chain is currently occluded (e.g., minimized or covered)
bool g_SwapChainOccluded = false;

// Width and height to resize the swap chain buffers when window size changes
UINT g_ResizeWidth = 0, g_ResizeHeight = 0;

// Render target view for the main render target
ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

// Handle to the window icon
HICON hIcon = nullptr;

// Flag to indicate if the window is currently being resized or moved (used to manage rendering pauses)
bool g_is_resizing_or_moving = false;

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

bool CreateDeviceD3D(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; 

	UINT createDeviceFlags = 0;
#if defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createDeviceFlags,
		featureLevels,
		_countof(featureLevels),
		D3D11_SDK_VERSION,
		&sd,
		&g_pSwapChain,
		&g_pd3dDevice,
		&featureLevel,
		&g_pd3dDeviceContext);

	if (FAILED(hr)) {
		std::cerr << "D3D11CreateDeviceAndSwapChain failed: 0x" << std::hex << hr << "\n";
		return false;
	}

	CreateRenderTarget();
	return true;
}

// Helper functions
void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain)
	{
		g_pSwapChain->Release();
		g_pSwapChain = nullptr;
	}
	if (g_pd3dDeviceContext)
	{
		g_pd3dDeviceContext->Release();
		g_pd3dDeviceContext = nullptr;
	}
	if (g_pd3dDevice)
	{
		g_pd3dDevice->Release();
		g_pd3dDevice = nullptr;
	}
}
void CleanupRenderTarget()
{
	if (g_mainRenderTargetView)
	{
		g_mainRenderTargetView->Release();
		g_mainRenderTargetView = nullptr;
	}
}


void CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer = nullptr;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	/*
	*  pBackBuffer - could be nullptr
	*/
	if (pBackBuffer == nullptr) {
		return;
	}
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
	pBackBuffer->Release();
}




