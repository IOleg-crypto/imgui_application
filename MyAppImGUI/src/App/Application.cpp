#include "Application.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

void Application::Init()
{
	m_Window.Init();
	HWND hwnd = m_Window.GetHWND();

	if (!CreateDeviceD3D(hwnd)) {
		CleanupDeviceD3D();
		return;
	}
	if (!g_pd3dDevice || !g_pd3dDeviceContext) {
		std::cerr << "Device or Device Context is null" << std::endl;
	}
	m_Window.InitImGui();

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
}
void Application::RunMainLoop(Application& app)
{
#if _DEBUG // NOLINT(clang-diagnostic-undef)
	std::setlocale(LC_ALL, "C.UTF-8");
	SetConsoleOutputCP(65001);
#endif
	while (!m_done)
	{
		m_Window.PollMessage(m_done);
		if (m_done) break;

		if (g_is_resizing_or_moving) {
			::Sleep(10);
			continue;
		}

		m_Window.HandleOcclusion(g_SwapChainOccluded, g_pSwapChain);
		m_Window.HandleResize(g_pSwapChain);
		m_TabManager.UpdateFontBeforeFrame();
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		DrawUI();
		m_TabManager.ShowFontWindow();
		ImGui::Render();
		constexpr float clear_color_with_alpha[4] = { 0,0,0,0 };
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		HRESULT hr = g_pSwapChain->Present(0, 0);
		g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
	}
#if _DEBUG
	std::cout << _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	std::cout << _CrtDumpMemoryLeaks();
#endif
}

HWND Application::GetHwnd()
{
	return m_Hwnd;
}

void Application::DrawUI()
{
	m_Window.ApplyFullscreenLayout(m_Window.windowFlags);
	if (ImGui::Begin("Notepad", &s_state.hideWindow , m_Window.windowFlags)) {
		// Stop program
		if (!s_state.hideWindow)
		{
			::PostQuitMessage(0);
		}
		m_TabManager.RenderMenuTab();
		m_TabManager.RenderInputTextField();
	}
	ImGui::End();
}






