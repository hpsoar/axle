#include "axle/ui/glut_window.h"

#include <string.h>

namespace ax {
GlutWindow::GlutWindow(const char *title, const int x, const int y, 
                       const int w, const int h, unsigned int mode) {
  glutInitDisplayMode(mode);
  glutInitWindowSize(w, h);

  glutInitWindowPosition(x, y);

  win_id_ = glutCreateWindow(title);
  if (win_id_ >= kMaxGlutWindowCount) {
    // output info
    exit(0);
  }
  if (win_id_ > 0) {
    //save a pointer to the stucture
    s_windows[win_id_] = this;
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialkey);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutMotionFunc(motion);
    glutPassiveMotionFunc(passive_motion);
    glutMouseFunc(button);
  } else {
    // output info
    exit(0);
  }
}

GlutWindow::~GlutWindow(void) {
  if (win_id_ > 0) {
    glutDestroyWindow(win_id_);
    s_windows[win_id_] = NULL;
  }
}

int GlutWindow::Run() { 
  glutMainLoop(); 
  return 0; 
}

void GlutWindow::RePaint() { glutPostRedisplay(); }
void GlutWindow::SetWindowSize(const int w, const int h) { 
  glutReshapeWindow(w, h); 
}

void GlutWindow::SetWindowPos(const int x, const int y) { 
  glutPositionWindow(x, y); 
}

void GlutWindow::SetWindowTitle(const char *title) {
  glutSetWindowTitle(title);
}

int GlutWindow::width() const {
  return glutGet(GLUT_WINDOW_WIDTH);  
}
int GlutWindow::height() const {
  return glutGet(GLUT_WINDOW_HEIGHT);
}
int GlutWindow::x() const {
  return glutGet(GLUT_WINDOW_X);
}

int GlutWindow::y() const {
  return glutGet(GLUT_WINDOW_Y);
}

inline void GlutWindow::button(int btn, int state, int x, int y) {
  int id = glutGetWindow();
  switch (btn) {
    case GLUT_LEFT_BUTTON:
      if (GLUT_UP == state) s_windows[id]->OnLeftBtnUp(x, y);
      else s_windows[id]->OnLeftBtnDown(x, y);
      break;
    case GLUT_MIDDLE_BUTTON:
      if (GLUT_UP == state) s_windows[id]->OnMiddleBtnUp(x, y);
      else s_windows[id]->OnMiddleBtnDown(x, y);
      break;
    case GLUT_RIGHT_BUTTON:
      if (GLUT_UP == state) s_windows[id]->OnRightBtnUp(x, y);
      else s_windows[id]->OnRightBtnDown(x, y);
      break;
    default:
      // Severe("not supported");
      break;
  }
}

inline void GlutWindow::display() {
  int id = glutGetWindow();
  s_windows[id]->OnPaint();
}

inline void GlutWindow::keyboard(unsigned char key, int x, int y) {
  int id = glutGetWindow();
  s_windows[id]->OnKeyDown(key, x, y);
}
inline void GlutWindow::specialkey(int key, int x, int y) {
  int id = glutGetWindow();
  s_windows[id]->OnKeyDown(MapSpecialKey(key), x, y);
}
inline void GlutWindow::idle() {
  int id = glutGetWindow();
  s_windows[id]->OnIdle();
}
inline void GlutWindow::reshape(int w, int h) {
  int id = glutGetWindow();
  s_windows[id]->OnResize(w, h);
}
inline void GlutWindow::motion(int x, int y) {
  int id = glutGetWindow();
  s_windows[id]->OnMouseMove(x, y);
}
inline void GlutWindow::passive_motion(int x, int y) {
}
inline void GlutWindow::timer(int millis) {
}
inline void GlutWindow::visibility(int state ){
}
inline void GlutWindow::menu_status(int state, int x, int y) {
}  
inline int GlutWindow::MapSpecialKey(int key) { return 0; }

GlutWindow* GlutWindow::s_windows[kMaxGlutWindowCount];

void DrawTextGL(int x, int y, const char *text) {
  int width = glutGet(GLUT_WINDOW_WIDTH);
  int height = glutGet(GLUT_WINDOW_HEIGHT);  

  glPushAttrib( GL_ALL_ATTRIB_BITS );
  glDisable(GL_DEPTH_TEST);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, width, 0, height);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glRasterPos2i(x, y);
  int len = strlen(text);
  for (int i = 0; i < len; ++i)
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *(text +i));

  //glRasterPos2i(0, 0);
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glPopAttrib();
  glFlush();
}

} // ax
