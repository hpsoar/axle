#ifndef AXLE_UI_GLUT_WINDOW_H
#define AXLE_UI_GLUT_WINDOW_H

#include "axle/core/settings.h"
#include "axle/ui/logic_window.h"
#include "axle/core/typedefs.h"

#if defined(SYS_IS_APPLE)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

namespace ax {
class GlutWindow : public LogicWindow {
public:
  GlutWindow(const char *title, const int x, const int y, 
             const int w, const int h, const uint32 mode);
  virtual ~GlutWindow(void);  
  static int Run();
  void RePaint();
  void SetWindowSize(const int width, const int height);
  void SetWindowPos(const int x, const int y);
  void SetWindowTitle(const char *title);
  int width() const;
  int height() const;
  int x() const;
  int y() const;

private:
  static void button(int btn, int state, int x, int y);
  static void display();
  static void keyboard(unsigned char key, int x, int y);
  static void specialkey(int key, int x, int y);
  static void idle();
  static void reshape(int w, int h);
  static void motion(int x, int y);
  static void passive_motion(int x, int y);
  static void timer(int millis);
  static void visibility(int state );
  static void menu_status(int state, int x, int y);
  static int MapSpecialKey(int key);
private:
  enum { kMaxGlutWindowCount = 100 };
  static GlutWindow* s_windows[kMaxGlutWindowCount];  
  int win_id_;
};

} // ax

#endif//AXLE_UI_GLUT_WINDOW_H

