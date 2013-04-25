#include "axle/ui/win32_window.h"

#ifdef SYS_IS_WINDOWS
#include <GL/glew.h>

namespace ax {
inline bool Win32Window::WinProc(Win32Window *window, UINT msg,
                                 WPARAM wParam, LPARAM lParam) {
  assert(NULL != window);
  if (NULL == window) return false;
  // Handle some specific messages:
	switch(msg)	{
	case WM_ERASEBKGND:
    return true;
	case WM_PAINT:
		window->OnPaint(wParam, lParam);
    return true;
	case WM_KEYDOWN:
		window->OnKeyDown(wParam, lParam);
    return true;
	case WM_KEYUP:	
		window->OnKeyUp(wParam, lParam);
    return true;
	case WM_LBUTTONDOWN:
		window->OnLButtonDown(wParam, lParam);
    return true;
	case WM_LBUTTONUP:
		window->OnLButtonUp(wParam, lParam);
    return true;
	case WM_RBUTTONDOWN:
		window->OnRButtonDown(wParam, lParam);
    return true;
	case WM_RBUTTONUP:
		window->OnRButtonUp(wParam, lParam);
    return true;
	case WM_MOUSELEAVE:
		window->OnMouseLeave(wParam, lParam);
    return true;
	case WM_MOUSEMOVE:
		window->OnMouseMove(wParam, lParam);
    return true;
	case WM_SIZE:	
		window->OnResize(wParam, lParam);
    return true;
	case  WM_COMMAND:	
		window->OnCommand(wParam, lParam);
    return true;
	case WM_HSCROLL:
		window->OnHScroll(wParam, lParam);
    return true;
	case WM_DESTROY:
    return true;
  default:
    break;
  }
  return false;
}

LRESULT CALLBACK Win32Window::WndProc(HWND hWnd, UINT msg, 
                                      WPARAM wParam, LPARAM lParam) {		
  if (WM_CLOSE == msg) {
		::PostQuitMessage(0); 
		return 0;
  } else if (msg == WM_CREATE) {
    if (NULL != Win32Window::s_CurrentWnd) {
		  Win32Window::s_CurrentWnd->OnCreate(hWnd, wParam, lParam);
      return 0;
    }
  } else {
	  Win32Window *window = Win32Window::FindWindow(hWnd);
    if(WinProc(window, msg, wParam, lParam)) return 0;
  }

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

Win32Window::~Win32Window(void) {	
	s_WindowMap.erase(_hWnd);
	DestroyWindow(_hWnd);
	_hWnd = NULL;
}

Win32Window::Win32Window(LPCSTR lpszClassName, int x, int y, int w, int h,
                         LPCSTR lpszWindowName, DWORD dwStyle, HWND hParentWnd,
                         HBRUSH hBackground, HICON hIcon, HCURSOR hCursor, 
                         LPCSTR lpszMenuName)
    : _hInstance(NULL), _hWnd(NULL) {
	HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);
	if (NULL == hInstance) return;
	_hInstance = hInstance;

	WNDCLASS wndClass = { 0 };

	wndClass.style = 0;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = hIcon ;
	wndClass.hCursor = hCursor;
	wndClass.hbrBackground = hBackground ;
	wndClass.lpszMenuName = lpszMenuName;
	wndClass.lpszClassName = lpszClassName;

	if (!RegisterClass(&wndClass)) {	
		DWORD code = GetLastError();
		if (1410 != code) {
			MessageBox(NULL, TEXT("RegisterClass failed!"), lpszClassName, 
                 MB_ICONERROR);
			return;
		}
	}

	//s_CurrentWnd is used only when creating the window
	s_CurrentWnd = this;
	_hWnd = CreateWindow(lpszClassName, 
		lpszWindowName,
		dwStyle,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		hParentWnd, NULL, hInstance, NULL);
	s_CurrentWnd = NULL;

	if (NULL == _hWnd) {		
		MessageBox(NULL, TEXT("CreateWindow failed"), lpszWindowName, MB_ICONERROR);
	} else {
		s_WindowMap[ _hWnd ] = this;
	}
	AllocConsole();
	freopen("conout$","w",stdout) ;
}

void Win32Window::Show(int nCmdShow) {
	if (NULL == _hWnd) return;
	::ShowWindow(_hWnd,  nCmdShow);
	::UpdateWindow(_hWnd);
}

void Win32Window::Show(void) {
	this->Show(SW_SHOW);	
}

void Win32Window::Hide(void) {
	if (NULL == _hWnd) return;
	::ShowWindow(_hWnd, SW_HIDE);
}

/************************************************************************
 * the following are static members
 ************************************************************************/
Win32Window::WindowMap Win32Window::s_WindowMap;
Win32Window* Win32Window::s_CurrentWnd = NULL;

Win32Window* Win32Window::FindWindow(HWND hWnd) {
	WindowMap::iterator it = s_WindowMap.find(hWnd);
	return s_WindowMap.end() == it ? NULL : it->second;
}
} // ui

#endif
