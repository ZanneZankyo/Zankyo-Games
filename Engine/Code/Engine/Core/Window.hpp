#pragma once

#include "Win32Include.hpp"
#include <string>
#include "Engine\Math\IntVector2.hpp"
#include "..\Math\AABB2.hpp"
#include "..\Input\InputSystem.hpp"

class RHIOutput;

typedef void(*WindowEventFunc)();

class Window
{
public:
	Window();
	Window(int width, int height);
	~Window();


	bool Open();
	bool IsOpen();
	bool IsClosed();
	void Close();
	void ProcessMessages(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam);
	void SetWindowSize(int width, int height);
	void SetWindowStyle(DWORD windowStyle);
	IntVector2 GetWindowSize();
	IntVector2 GetClientSize();
	void RegisterCustomMessageHandler(void* callBack);
	void CenterWindow();
	void BorderlessFullScreen();
	void TriggerFullScreen();
	void RunMessagePump();
	void SetWindowTitle(std::string title);
	void LockMouse();
	void ResizeWindow(int width, int height);
	IntVector2 GetPosition();
	//ReturnType (*FunctionName)(Args);
	static std::string GetWindowText(HWND windowHandle);
	static IntVector2 GetDesktopResolution();
	static HWND GetWindowHandle(const std::string& windowName);
	static bool GetWindowBounds(const std::string& windowName, AABB2* outBounds);
	static bool GetWindowBounds(HWND windowHandle, AABB2* outBounds);
	static void SendMessage(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam);
	static void SendInput(HWND windowHandle, KeyCode keycode);

public:
	WindowEventFunc e_lostFocus = nullptr;
	WindowEventFunc e_getFocus = nullptr;

public:

	HWND m_windowHandle;
	RHIOutput* m_rhiOutput;
private:
	HDC m_displayDeviceContext;
	
	bool m_isOpen;
	std::string	m_windowTitle;
	IntVector2 m_windowSize;
	IntVector2 m_contentSize;
	IntVector2 m_windowPos;
	void (*foo) (int);
};

void RunMessagePump();

inline Window::Window()
	: m_windowHandle(nullptr)
	, m_displayDeviceContext(nullptr)
	, m_windowTitle("Zankyo Engine")
	, m_windowSize(1280, 720)
	, m_windowPos(50, 50)
	, m_isOpen(true)
	, m_rhiOutput(nullptr)
{
	Open();
}

inline Window::Window(int width, int height)
	: m_windowHandle(nullptr)
	, m_displayDeviceContext(nullptr)
	, m_windowTitle("Zankyo Engine")
	, m_windowSize(width, height)
	, m_windowPos(50, 50)
	, m_isOpen(true)
	, m_rhiOutput(nullptr)
{
	Open();
}

inline Window::~Window()
{}

inline bool Window::IsOpen()
{
	return m_isOpen;
}

inline bool Window::IsClosed()
{
	return !m_isOpen;
}

inline void Window::Close()
{
	m_isOpen = false;
}

