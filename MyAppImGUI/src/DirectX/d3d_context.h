#ifndef D3D_CONTEXT_H 
#define D3D_CONTEXT_H

#include <d3d11.h>

// Direct3D device pointer
static ID3D11Device* g_pd3dDevice = nullptr;

// Direct3D device context pointer
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;

// Swap chain for presenting rendered frames
static IDXGISwapChain* g_pSwapChain = nullptr;

// Flag indicating if the swap chain is currently occluded (e.g., minimized or covered)
static bool g_SwapChainOccluded = false;

// Width and height to resize the swap chain buffers when window size changes
static UINT g_ResizeWidth = 0, g_ResizeHeight = 0;

// Render target view for the main render target
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

// Handle to the window icon
static HICON hIcon = nullptr;

// Flag to indicate if the window is currently being resized or moved (used to manage rendering pauses)
static bool g_is_resizing_or_moving = false;

// Window procedure callback function to handle window messages
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Helper function: Creates and initializes the Direct3D device and swap chain for the given window
static bool CreateDeviceD3D(const HWND& hWnd);

// Helper function: Cleans up and releases Direct3D device and associated resources
void CleanupDeviceD3D();

// Helper function: Creates the render target view for the swap chain back buffer
void CreateRenderTarget();

// Helper function: Releases the render target view and related resources
void CleanupRenderTarget();

#endif
