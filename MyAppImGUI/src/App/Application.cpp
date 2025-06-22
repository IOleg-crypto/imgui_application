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
}

void Application::DrawUI()
{
	static UIState ui;
	ImGui::SetNextWindowSize(ImVec2(600, 480), ImGuiCond_Once);
	if (ImGui::Begin("Notepad", &ui.hideWindow , ui.windowFlags)) {
		// Stop program
		if (!ui.hideWindow)
		{
			::PostQuitMessage(0);
		}
		m_TabManager.RenderMenuTab();
		m_TabManager.RenderInputTextField();
	}
	ImGui::End();
}




