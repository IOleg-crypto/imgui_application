#ifndef D3D_CONTEXT_H 
#define D3D_CONTEXT_H

#include <d3d11.h>

extern ID3D11Device* g_pd3dDevice;
extern ID3D11DeviceContext* g_pd3dDeviceContext;
extern IDXGISwapChain* g_pSwapChain;
extern bool                    g_SwapChainOccluded;
extern UINT                    g_ResizeWidth;
extern UINT                    g_ResizeHeight;
extern ID3D11RenderTargetView* g_mainRenderTargetView;
extern bool                    g_is_resizing_or_moving;

// Window procedure callback function to handle window messages
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Helper function: Creates and initializes the Direct3D device and swap chain for the given window
bool CreateDeviceD3D(HWND hWnd);

// Helper function: Cleans up and releases Direct3D device and associated resources
void CleanupDeviceD3D();

// Helper function: Creates the render target view for the swap chain back buffer
void CreateRenderTarget();

// Helper function: Releases the render target view and related resources
void CleanupRenderTarget();

#endif
