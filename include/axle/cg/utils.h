#ifndef AXLE_GRAPHICS_UTILS_H
#define AXLE_GRAPHICS_UTILS_H

#include "axle/core/settings.h"
#include "axle/core/typedefs.h"
#include "axle/core/math.h"
#include "axle/geom.h"

#include <GL/glew.h>
#include "axle/cg/cg_fwd.h"

namespace ax {
class ArrayBufferGL {
public:
  ArrayBufferGL() : id_(0), gpu_ptr_(0), size_(0) { }
  ~ArrayBufferGL() { this->Release(); }    
  bool Initialize(uint32 size, uint32 access, const void *data = NULL);

  void SetData(int offset, int size, const void *data);
  GLuint id() const { return this->id_; }
  GLuint64EXT gpu_ptr() const { return this->gpu_ptr_; }
  uint32 size() const { return this->size_; }
private:
  void Release();
private:
  GLuint id_;
  GLuint64EXT gpu_ptr_;
  uint32 size_;
};

void DrawQuad(float x1, float y1, float x2, float y2);
void DrawQuad(float x1, float y1, float z1,
              float x2, float y2, float z2,
              float x3, float y3, float z3);
inline void DrawQuad() { DrawQuad(0, 0, 1, 1); }
inline void DrawQuad(float size) {
  DrawQuad(0, 0, size, size);
}
void DrawQuad(float x1, float y1, float x2, float y2, float z);
void DrawCube(float x1, float y1, float z1,
              float x2, float y2, float z2);
void DrawTextGL(int x, int y, const char *text);
void DisplayStatistics(const char *name, float data, const char *unit,
                       int x = 2, int y = 9);
GLuint MakeFullScreenQuad();

void VisualizeTexture(const Texture2DPtr tex);
void SaveFrameBuffer(const char *filename);
/*
 * returns ture if error ocurred, otherwise return false
 */
bool CheckErrorsGL(const char *location);

bool CheckGLInfo();

template<typename T>
void EmitVertex(const T &p) { glVertex3f(p.x, p.y, p.z); }

template<typename C>
void VisualizeViewSpace(const C &camera) {
  float fovy_2 = ax::deg2rad(camera.fovy()) * 0.5f;
  float tan_fovy_2 = tan(fovy_2);
  float near_half_len = tan_fovy_2 * camera.z_near();
  float far_half_len = tan_fovy_2 * camera.z_far();
  float x[] = { -1, 1, 1, -1 };
  float y[] = { -1, -1, 1, 1 };
  ax::Vector3 far_plane[4];
  ax::Vector3 near_plane[4];
  for (int i = 0; i < 4; ++i) {
    far_plane[i] = ax::Vector3(x[i] * far_half_len,
                                 y[i] * far_half_len,
                                 camera.z_far());
    near_plane[i] = ax::Vector3(x[i] * near_half_len,
                                  y[i] * near_half_len,
                                  camera.z_near());
  }

  /*glEnable(GL_BLEND);
  
  glDisable(GL_CULL_FACE);*/
  glColor3f(1, 1, 1);  
  glPushMatrix();

  glTranslatef(camera.position().x,
               camera.position().y,
               camera.position().z);
  ax::Vector3 look = camera.look();
  float x_angle = ax::rad2deg(atan2(look.x, look.z));
  float y_angle = ax::rad2deg(atan2(look.y, look.z));
  glRotatef(x_angle, 0, 1, 0);
  glRotatef(y_angle + 180, 1, 0, 0);

  /*glutSolidSphere(0.2, 32, 32);*/
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glBegin(GL_QUADS);
  for (int i = 0; i < 4; ++i) EmitVertex(near_plane[i]);

  for (int i = 0; i < 4; ++i) EmitVertex(far_plane[3 - i]);

  glColor3f(1, 0, 0);
  EmitVertex(near_plane[0]);
  EmitVertex(near_plane[3]);
  EmitVertex(far_plane[3]);
  EmitVertex(far_plane[0]);

  glColor3f(0, 1, 0);
  EmitVertex(near_plane[3]);
  EmitVertex(near_plane[2]);
  EmitVertex(far_plane[2]);
  EmitVertex(far_plane[3]);

  glColor3f(0, 0, 1);
  EmitVertex(near_plane[2]);
  EmitVertex(near_plane[1]);
  EmitVertex(far_plane[1]);
  EmitVertex(far_plane[2]);

  glColor3f(0.8f, 0.5f, 0.2f);
  EmitVertex(near_plane[1]);
  EmitVertex(near_plane[0]);
  EmitVertex(far_plane[0]);
  EmitVertex(far_plane[1]);
  
  glEnd();
  //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glPopMatrix();
  glBegin(GL_LINES);
  EmitVertex(camera.position());
  EmitVertex(camera.look() * 100.f);
  glEnd();
}
} // ax

#endif // AXLE_GRAPHICS_UTILS_H
