#ifndef AXLE_UI_LOGICAL_WINDOW_H
#define AXLE_UI_LOGICAL_WINDOW_H

namespace ax {
class LogicWindow {
public:  
  virtual ~LogicWindow() = 0;
  virtual int width() const = 0;
  virtual int height() const = 0;
  virtual int x() const = 0;
  virtual int y() const = 0;
  virtual void SetWindowTitle(const char *) = 0;
protected:
  virtual void OnPaint() { }
  virtual void OnResize(int width, int height) {  }
  virtual void OnIdle() { }
  virtual void OnKeyUp(int key, int x, int y) { }
  virtual void OnKeyDown(int key, int x, int y) { }
  virtual void OnLeftBtnDown(int x, int y) { }
  virtual void OnLeftBtnUp(int x, int y) { }
  virtual void OnRightBtnDown(int x, int y) { }
  virtual void OnRightBtnUp(int x, int y) { }
  virtual void OnMiddleBtnDown(int x, int y) { }
  virtual void OnMiddleBtnUp(int x, int y) { }
  virtual void OnMouseWheel(int x, int y, int dz, int flag) { }
  virtual void OnMouseMove(int x, int y) { }
  virtual void OnCommand(int cmd) { }
};
} // ax

#endif // AXLE_UI_LOGICAL_WINDOW_H
