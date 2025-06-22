#include "Window.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "d3d_context.h"
#include <iostream>

Window::Window() : m_iconPath(L"assets/icon/icon.ico"), m_fontPath(R"(C:\Windows\Fonts\Arial.ttf)"), m_fullscreen(false)
{

}
Window::Window(HWND &hwnd)
	: m_iconPath(L"assets/icon/icon.ico"), m_fontPath(R"(C:\Windows\Fonts\Arial.ttf)"), m_hwnd(hwnd), m_fullscreen(false) 
{
	
}


Window::~Window()
{
	if (m_classRegistered) {
		UnregisterClassW(m_wc.lpszClassName, m_wc.hInstance);
	}
}

void Window::Init(){
	
	m_hIcon = static_cast<HICON>(LoadImageW(nullptr, m_iconPath.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE));
	if (!m_hIcon) {
		MessageBoxW(nullptr, L"Failed to load icon", L"Error", MB_ICONERROR);
		return;
	}
	ZeroMemory(&m_wc, sizeof(m_wc));
	m_wc.cbSize = sizeof(WNDCLASSEXW);
	m_wc.style = CS_HREDRAW | CS_VREDRAW;
	m_wc.lpfnWndProc = WndProc;
	m_wc.cbClsExtra = 0;
	m_wc.cbWndExtra = 0;
	m_wc.hInstance = GetModuleHandle(nullptr);
	m_wc.hIcon = m_hIcon;
	m_wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	m_wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	m_wc.lpszMenuName = nullptr;
	m_wc.lpszClassName = L"Notepad";
	m_wc.hIconSm = nullptr;

	if (!RegisterClassExW(&m_wc)) {
		MessageBoxW(nullptr, L"Failed to register window class", L"Error", MB_ICONERROR);
		m_classRegistered = false;
		return;
	}

	m_hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST, m_wc.lpszClassName, L"Notepad",
		WS_POPUP, 0, 40, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
		nullptr, nullptr, m_wc.hInstance, nullptr);

	SetLayeredWindowAttributes(m_hwnd, 0, 255, LWA_COLORKEY);

	if (!m_hwnd) {
		MessageBoxW(nullptr, L"Failed to create window", L"Error", MB_ICONERROR);
		return;
	}
}

void Window::ToggleFullscreen() {
	m_fullscreen = !m_fullscreen;
}

void Window::AboutWindow(bool& showDemoWindow)
{
	ImGuiIO &io = ImGui::GetIO();
	if (ImGui::Begin("##About", &showDemoWindow))
	{
		ImGui::Text("The notepad made by I#Oleg");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
	}
	ImGui::End();
}

void Window::PollMessage(bool& done) const {
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_QUIT) done = true;
	}
}

void Window::ApplyFullscreenLayout(ImGuiWindowFlags& outFlags) {
	ImGuiIO& io = ImGui::GetIO();
	static bool prevFullscreen = false;
	ImGuiCond posCond = ImGuiCond_Appearing;
	ImGuiCond sizeCond = ImGuiCond_Appearing;

	if (m_fullscreen != prevFullscreen) {
		posCond = ImGuiCond_Always;
		sizeCond = ImGuiCond_Always;
		prevFullscreen = m_fullscreen;
	}

	if (m_fullscreen) {
		ImGui::SetNextWindowPos(ImVec2(0, 0), posCond);
		ImGui::SetNextWindowSize(io.DisplaySize, sizeCond);
		outFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar;
	}
	else {
		ImVec2 windowSize(800, 400);
		ImVec2 centerPos((io.DisplaySize.x - windowSize.x) * 0.5f, (io.DisplaySize.y - windowSize.y) * 0.5f);
		ImGui::SetNextWindowPos(centerPos, posCond);
		ImGui::SetNextWindowSize(windowSize, sizeCond);
		outFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoCollapse;
	}
}
void Window::HandleResize(IDXGISwapChain* swapChain) const
{
	if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
	{
		CleanupRenderTarget();
		swapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
		g_ResizeWidth = g_ResizeHeight = 0;
		CreateRenderTarget();
	}
}
void Window::HandleOcclusion(bool& g_SwapChainOccluded, IDXGISwapChain* swapChain) const
{
	if (g_SwapChainOccluded && swapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
	{
		::Sleep(10);
		return;
	}
	g_SwapChainOccluded = false;
}

void Window::InitImGui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	m_io = io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Page Controls
	io.WantCaptureMouse = true;
	// To support cyrillic
	io.Fonts->AddFontFromFileTTF(m_fontPath.c_str(), 20, nullptr, io.Fonts->GetGlyphRangesCyrillic());
}
	

