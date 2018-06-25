#include "Window.hpp"
#include <winuser.h>
#include "EngineConfig.hpp"
#include "Engine\Core\EngineConfig.hpp"
#include "EngineBase.hpp"
#include "Engine\Input\InputSystem.hpp"
#include <dxgi.h>
//#include "Game\Game.hpp"
#include "Config.hpp"
#include "Engine\RHI\RHIOutput.hpp"

// For DPI Awareness
#include <ShellScalingApi.h>
#include <stdlib.h>
#include <tchar.h>
#include "StringUtils.hpp"
#pragma comment(lib, "Shcore.lib")

static char const*const WNDCLASS_GAME_NAME = "Zankyo Engine";

LRESULT CALLBACK ProcessWindowMessage(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam)
{
	Window *window = (Window*)GetWindowLongPtr(windowHandle, GWLP_USERDATA);
	if (window != nullptr && window->IsOpen())
	{
		window->ProcessMessages(windowHandle, wmMessageCode, wParam, lParam);
		//window->RegisterCustomMessageHandler(NullFuncPtr);
	}
	return DefWindowProc(windowHandle, wmMessageCode, wParam, lParam);
}

static HINSTANCE GetCurrentHINSTANCE()
{
	return GetModuleHandle(NULL);
}

void Window::RunMessagePump()
{
	MSG queuedMessage;
	for (;; )
	{
		const BOOL wasMessagePresent = PeekMessage(&queuedMessage, NULL, 0, 0, PM_REMOVE);
		if (!wasMessagePresent)
			break;

		TranslateMessage(&queuedMessage);
		DispatchMessage(&queuedMessage);
	}
}

void Window::SetWindowTitle(std::string title)
{
	std::wstring wtitle(title.size(), '#');
	//mbstowcs(&wtitle[0], title.c_str(), title.size());
	size_t size;
	mbstowcs_s(&size, &wtitle[0], wtitle.size(), title.c_str(), title.size());
	SetWindowText(m_windowHandle, wtitle.c_str());
}

void Window::LockMouse()
{
	RECT rect;
	GetWindowRect(m_windowHandle, &rect);
	ClipCursor(&rect);
}

void Window::ResizeWindow(int width, int height)
{
	if (m_rhiOutput)
	{
		RECT rect;
		GetClientRect(m_windowHandle, &rect);
		m_contentSize = IntVector2(rect.right, rect.bottom);
		//m_rhiOutput->m_device->m_immediateContext->RSSetViewports(rect.top, rect.left, rect.right - rect.left, rect.bottom - rect.top);
		m_rhiOutput->ResizeBuffer(width, height);
		m_windowSize = IntVector2(width, height);
		//ShowWindow(m_windowHandle, SW_SHOW);
	}
}

IntVector2 Window::GetPosition()
{
	return m_windowPos;
	/*RECT rect;
	HRESULT hr = GetWindowRect(m_windowHandle, &rect);
	if (hr)
	{
		return IntVector2(rect.left, rect.top);
	}
	return IntVector2();*/
}

std::string Window::GetWindowText(HWND windowHandle)
{
	LPTSTR lpString = new WCHAR[100];
	int nMaxCount = 100;
	::GetWindowText(
		windowHandle,
		lpString,
		nMaxCount
	);
	return StringUtils::ToString(std::wstring(lpString));
}

IntVector2 Window::GetDesktopResolution()
{
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	//horizontal = desktop.right;
	//vertical = desktop.bottom;

	return IntVector2(desktop.right, desktop.bottom);
}

HWND Window::GetWindowHandle(const std::string& windowName)
{
	std::wstring wstr = StringUtils::ToWstring(windowName);
	HWND hWnd = ::FindWindow(0, wstr.c_str());
	return hWnd;
}

bool Window::GetWindowBounds(const std::string& windowName, AABB2* outBounds)
{
	HWND hWnd = GetWindowHandle(windowName);
	return GetWindowBounds(hWnd, outBounds);
}

bool Window::GetWindowBounds(HWND windowHandle, AABB2* outBounds)
{
	if (!windowHandle)
		return false;
	RECT rect;
	GetWindowRect(windowHandle, &rect);
	outBounds->mins = Vector2(rect.top, rect.left);
	outBounds->maxs = Vector2(rect.right, rect.bottom);
	return true;
}

void Window::SendMessage(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam)
{
	::SendMessage(
		windowHandle,
		wmMessageCode,
		wParam,
		lParam
	);
}

bool Window::Open()
{
	SetProcessDPIAware();

	WNDCLASSEX windowClassDescription;
	memset(&windowClassDescription, 0, sizeof(windowClassDescription));
	windowClassDescription.cbSize = sizeof(windowClassDescription);
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast<WNDPROC>(ProcessWindowMessage); // Assign a win32 message-handling function
	windowClassDescription.hInstance = GetModuleHandle(NULL);
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = TEXT("Zankyo Engine");
	RegisterClassEx(&windowClassDescription);

	const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_OVERLAPPED;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect(desktopWindowHandle, &desktopRect);

	//get desktop resolution
	int desktopWidth = desktopRect.right - desktopRect.left;
	int desktopHeight = desktopRect.bottom - desktopRect.top;

	int windowWidth = m_windowSize.x;//windowHeight * m_windowSize.x / m_windowSize.y;
	int windowHeight = m_windowSize.y;//(int)(0.85f * (float)desktopHeight);
// 	Config::GetInt(windowHeight,"window_height" );
// 	Config::GetInt(windowWidth,"window_width" );

	m_windowSize = IntVector2(windowWidth, windowHeight);
	m_contentSize = IntVector2(windowWidth, windowHeight);

	int windowOffsetX = (desktopWidth - windowWidth) / 2;
	int windowOffsetY = (desktopHeight - windowHeight) / 2;

	int left = windowOffsetX;
	int right = left + windowWidth;
	int top = windowOffsetY;
	int bottom = top + windowHeight;

	m_windowPos = IntVector2(left, top);

	

	RECT windowRect = { left, top, right, bottom };
	AdjustWindowRectEx(&windowRect, windowStyleFlags, FALSE, windowStyleExFlags); //not affected by client border;

	

	m_windowTitle = g_windowTitle;

	WCHAR windowTitle[1024];
	MultiByteToWideChar(GetACP(), 0, m_windowTitle.c_str(), -1, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[0]));
	m_windowHandle = CreateWindowEx(
		windowStyleExFlags,
		windowClassDescription.lpszClassName,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		/*left,top,windowWidth,windowHeight,*/
		/*left,top,right-left,bottom-top,*/
		NULL,
		NULL,
		GetCurrentHINSTANCE(),
		NULL);

	ShowWindow(m_windowHandle, SW_SHOW);
	SetForegroundWindow(m_windowHandle);
	SetFocus(m_windowHandle);
	SetWindowLongPtr(m_windowHandle, GWLP_USERDATA, (LONG_PTR)this);

	m_displayDeviceContext = GetDC(m_windowHandle);

	HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
	SetCursor(cursor);

	//CenterWindow();
	//SetWindowPos(m_windowHandle, HWND_NOTOPMOST, left, top, windowWidth, windowHeight, SWP_FRAMECHANGED | SWP_SHOWWINDOW);

	return true;
}

void Window::ProcessMessages(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam)
{
	UNUSED(windowHandle);
	UNUSED(lParam);
	unsigned char asKey = (unsigned char)wParam;
	switch (wmMessageCode)
	{

	case WM_CLOSE:
	case WM_DESTROY:
	case WM_QUIT:
		Close();
		break;

	case WM_KEYDOWN:
		InputSystem::RegisterKeyDown(asKey);
		break;
	case WM_KEYUP:
		InputSystem::RegisterKeyUp(asKey);
		break;
	case WM_CHAR:
		InputSystem::RegisterKeyChar(asKey);
		break;

	case WM_LBUTTONDOWN:
		InputSystem::RegisterKeyDown(KEYCODE_LMB);
		break;
	case WM_LBUTTONUP:
		InputSystem::RegisterKeyUp(KEYCODE_LMB);
		break;

	case WM_RBUTTONDOWN:
		InputSystem::RegisterKeyDown(KEYCODE_RMB);
		break;
	case WM_RBUTTONUP:
		InputSystem::RegisterKeyUp(KEYCODE_RMB);
		break;

	case WM_MBUTTONDOWN:
		InputSystem::RegisterKeyDown(KEYCODE_MMB);
		break;
	case WM_MBUTTONUP:
		InputSystem::RegisterKeyUp(KEYCODE_MMB);
		break;
	case WM_SETFOCUS:
		InputSystem::OnGainFocus();
		if(e_getFocus != nullptr)
			e_getFocus();
		break;

	case WM_KILLFOCUS:
		InputSystem::OnLostFocus();
		if(e_lostFocus != nullptr)
			e_lostFocus();
		break;
	case WM_SIZE:
	{
		short* windowSizes = (short*)&lParam;
		int width = windowSizes[0];
		int height = windowSizes[1];
		ResizeWindow(width, height);
		break;
	}
		

	case WM_MOUSEWHEEL:
	{
		short wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		if (wheelDelta > 0)
			InputSystem::RegisterKeyDown(KEYCODE_WHEEL_UP);
		else if (wheelDelta < 0)
			InputSystem::RegisterKeyDown(KEYCODE_WHEEL_DOWN);
		break;
	}
	default:
		break;

	}
}

void Window::SetWindowSize(int width, int height)
{
	RECT rect;
	GetWindowRect(m_windowHandle, &rect);
	SetWindowPos(m_windowHandle, HWND_NOTOPMOST, rect.left, rect.top, width, height, SWP_SHOWWINDOW);
	//m_rhiOutput->
}

void Window::SetWindowStyle(DWORD windowStyle)
{
	SetWindowLong(m_windowHandle, GWL_STYLE, windowStyle);
}

IntVector2 Window::GetWindowSize()
{
	return m_windowSize;
}

IntVector2 Window::GetClientSize()
{
	RECT rect;
	GetClientRect(m_windowHandle, &rect);
	return IntVector2(rect.right, rect.bottom);
	//return m_contentSize;
}

void Window::RegisterCustomMessageHandler(void * callBack)
{
	UNUSED(callBack);
}

void Window::CenterWindow()
{

	const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_OVERLAPPED;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;
	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect(desktopWindowHandle, &desktopRect);

	//get desktop resolution
	int desktopWidth = desktopRect.right - desktopRect.left;
	int desktopHeight = desktopRect.bottom - desktopRect.top;

	int windowHeight = m_windowSize.y;
	int windowWidth = m_windowSize.x;

	Config::GetInt(windowWidth, "window_width");
	Config::GetInt(windowHeight, "window_height");

	m_windowSize = IntVector2(windowWidth, windowHeight);

	int windowOffsetX = (desktopWidth - windowWidth) / 2;
	int windowOffsetY = (desktopHeight - windowHeight) / 2;

	int left = windowOffsetX;
	int right = left + windowWidth;
	int top = windowOffsetY;
	int bottom = top + windowHeight;

	m_windowPos = IntVector2(left, top);
	m_contentSize = IntVector2(windowOffsetX, windowOffsetY);

	RECT windowRect = { left, top, right, bottom };

	AdjustWindowRectEx(&windowRect, windowStyleFlags, FALSE, windowStyleExFlags); //not affected by client border;

	m_rhiOutput->m_swapChain->SetFullscreenState(FALSE, NULL);

	SetWindowLong(m_windowHandle, GWL_STYLE, windowStyleFlags);
	SetWindowLong(m_windowHandle, GWL_EXSTYLE, windowStyleExFlags);

	SetWindowPos(m_windowHandle, HWND_NOTOPMOST, windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, SWP_FRAMECHANGED | SWP_SHOWWINDOW);
	//AdjustWindowRectEx(&windowRect, windowStyleFlags, FALSE, windowStyleExFlags);
	
}

void Window::BorderlessFullScreen()
{
	const DWORD windowStyleFlags = WS_POPUP;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;
	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect(desktopWindowHandle, &desktopRect);

	//get desktop resolution
	int desktopWidth = desktopRect.right - desktopRect.left;
	int desktopHeight = desktopRect.bottom - desktopRect.top;

	m_windowPos = IntVector2(0, 0);

	RECT windowRect = { 0, 0, desktopWidth, desktopHeight };
	AdjustWindowRectEx(&windowRect, windowStyleFlags, FALSE, windowStyleExFlags); //not affected by client border;
	//SetWindowStyle(windowStyleFlags);

	LONG lStyle = GetWindowLong(m_windowHandle, GWL_STYLE);
	lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
	SetWindowLong(m_windowHandle, GWL_STYLE, lStyle);

	LONG lExStyle = GetWindowLong(m_windowHandle, GWL_EXSTYLE);
	lExStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);

	m_rhiOutput->m_swapChain->SetFullscreenState(FALSE, NULL);

	SetWindowLong(m_windowHandle, GWL_EXSTYLE, lExStyle);

	SetWindowPos(m_windowHandle, HWND_NOTOPMOST, windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, SWP_FRAMECHANGED | SWP_SHOWWINDOW);

	//SetWindowPos(m_windowHandle, HWND_TOPMOST, 0, 0, desktopWidth, desktopHeight, SWP_FRAMECHANGED | SWP_SHOWWINDOW);
	//AdjustWindowRectEx(&windowRect, windowStyleFlags, FALSE, windowStyleExFlags);
}

void Window::TriggerFullScreen()
{
	IDXGIOutput* output;
	m_rhiOutput->m_swapChain->GetContainingOutput(&output);
	BOOL isFullScreen;
	m_rhiOutput->m_swapChain->GetFullscreenState(&isFullScreen, &output);
	if (isFullScreen == TRUE)
	{
		m_rhiOutput->m_swapChain->SetFullscreenState(FALSE, NULL);
	}

	else
	{
		m_rhiOutput->m_swapChain->SetFullscreenState(TRUE, output);
	}
}
