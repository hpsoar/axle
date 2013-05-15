#ifndef AXLE_UI_WIN32_WINDOW_H
#define AXLE_UI_WIN32_WINDOW_H

#include "axle/core/settings.h"

#ifdef SYS_IS_WINDOWS
#include <windows.h>
#include <map>
#include "axle/core/macros.h"
#include "axle/ui/logic_window.h"

namespace ax {
class Win32Window : public LogicWindow {
public:
	Win32Window(LPCSTR lpszClassName,               
              int x, int y, int w, int h,
              LPCSTR lpszWindowName = NULL,
              DWORD dwStyle = WS_OVERLAPPEDWINDOW, 
              HWND hParrentWnd = NULL,	
              HBRUSH hBackground = (HBRUSH)(COLOR_WINDOW), 
              HICON hIcon = NULL, 
              HCURSOR hCursor = LoadCursor(NULL, IDC_ARROW), 
              LPCSTR lpszMenuName = NULL);

	virtual ~Win32Window(void);
	virtual void Show(int nCmdShow);
	virtual void Show(void);
	virtual void Hide(void);

private:
	void OnLButtonDown(WPARAM wParam, LPARAM lParam) { 
    int xPos = LOWORD(lParam); 
    int yPos = HIWORD(lParam); 
    this->OnLeftBtnDown(xPos, yPos);
  }
	void OnLButtonUp(WPARAM wParam, LPARAM lParam) {
    int xPos = LOWORD(lParam); 
    int yPos = HIWORD(lParam); 
    this->OnLeftBtnUp(xPos, yPos);
  }
	void OnRButtonDown(WPARAM wParam, LPARAM lParam) {
    int xPos = LOWORD(lParam); 
    int yPos = HIWORD(lParam); 
    this->OnRightBtnDown(xPos, yPos);
  }
	void OnRButtonUp(WPARAM wParam, LPARAM lParam) { 
    int xPos = LOWORD(lParam); 
    int yPos = HIWORD(lParam); 
    this->OnRightBtnUp(xPos, yPos);
  }
	void OnMouseMove(WPARAM wParam, LPARAM lParam) {
    int xPos = LOWORD(lParam); 
    int yPos = HIWORD(lParam); 
    int btn = 0;
    switch(wParam) {
    case 0:
      break;
    default:
      break;
    }
    LogicWindow::OnMouseMove(xPos, yPos);
  }
	void OnMouseLeave(WPARAM wParam, LPARAM lParam) {
    //LogicWindow::OnMouseLeave();
  }
	void OnKeyDown(WPARAM wParam, LPARAM lParam) {
    int key = MapKey(wParam);
    LogicWindow::OnKeyDown(key, 0, 0);
  }
	void OnKeyUp(WPARAM wParam, LPARAM lParam) {
    int key = MapKey(wParam);
    LogicWindow::OnKeyUp(key, 0, 0);
  }
	void OnResize(WPARAM wParam, LPARAM lParam) {
    int cx = LOWORD(lParam);
    int cy = HIWORD(lParam);
    LogicWindow::OnResize(cx, cy);
  }
	void OnPaint(WPARAM wParam, LPARAM lParam) {
    LogicWindow::OnPaint();
  }
	void OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam) {
  }
	void OnCommand(WPARAM wParam, LPARAM lParam) {
    int cmd = 0;
    LogicWindow::OnCommand(cmd);
  }
	void OnHScroll(WPARAM wParam, LPARAM lParam) {
  }
  void OnVScroll(WPARAM wParam, LPARAM lParam) {
  }

private:
	DISABLE_COPY_AND_ASSIGN(Win32Window);
  int MapKey(WPARAM wParam) { return 0; }

protected:
	typedef std::map<HWND, Win32Window*> WindowMap;
	HWND _hWnd;
	HINSTANCE _hInstance;	
	
	static WindowMap s_WindowMap;
	static Win32Window* FindWindow(HWND hWnd);
	static Win32Window* s_CurrentWnd;	

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, 
                                  WPARAM wParam, LPARAM lParam);
  static bool Win32Window::WinProc(Win32Window *window, UINT msg,
                                   WPARAM wParam, LPARAM lParam);
};
} // ax

#endif

#endif//AXLE_UI_WIN32_WINDOW_H
