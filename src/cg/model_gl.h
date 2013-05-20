#ifndef AXLE_CG_CG_GL_H
#define AXLE_CG_CG_GL_H

#include "../core.h"
#include "../model/triangle_mesh.h"
#include "../geom.h"
#include "cg_fwd.h"

namespace ax {
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

/* 
 * The implementation should make sure that sizeof(Normal) == sizeof(Position)
 */

class Quad3DGL {
public:  
  Quad3DGL(const float x1, const float y1, const float z1,
         const float x2, const float y2, const float z2,
         const float nx, const float ny, const float nz);
  Quad3DGL(const Point &p1, const Point &p2, const Normal &n);
private:
  float vertices_[12];
  float tcoords_[8];
  static const float indices[6];
};

class Quad2DGL {
public:  
  Quad2DGL(const float x1, const float y1, const float x2, const float y2) {
    CreateQuad2D(x1, y1, x2, y2);
  }
  Quad2DGL(const Point &p1, const Point &p2) {
    CreateQuad2D(p1.x, p1.y, p2.x, p2.y);
  }

  virtual ~Quad2DGL() { }

  void set_tcoords(const float *tcoords) {
    memcpy(tcoords_, tcoords, sizeof(tcoords_));
  }
  void set_tcoords(const int *tcoords) {
    memcpy(tcoords_, tcoords, sizeof(tcoords_));
  }
  void set_vertex_slot(const int slot) { vertex_slot_ = slot; }
  void set_tcoord_slot(const int slot) { tcoord_slot_ = slot; }
  void Draw();
private:
  void CreateQuad2D(const float x1, const float y1, 
                    const float x2, const float y2) {
    vertices_[0] = x1; vertices_[1] = y1; // left-bottom
    vertices_[2] = x2; vertices_[3] = y1; // right-bottom
    vertices_[4] = x2; vertices_[5] = y2; // right-top
    vertices_[6] = x1; vertices_[7] = y2; // left-top
    vertex_slot_ = 0; tcoord_slot_ = 1;
    tcoords_[0] = 0.f; tcoords_[1] = 0.f; 
    tcoords_[2] = 1.f; tcoords_[3] = 0.f; 
    tcoords_[4] = 1.f; tcoords_[5] = 1.f; 
    tcoords_[6] = 0.f; tcoords_[7] = 1.f;
    CreateVBO();
  }
  void CreateVBO();
private:
  int vertex_slot_, tcoord_slot_;
  float vertices_[8];
  union {
    float tcoords_[8];
    int i_tcoords_[8];
  };
  uint32 vert_vbo_;
  uint32 idx_vbo_;
  static const uint32 indices_[6];
};

class ScreenQuad;
typedef std::tr1::shared_ptr<ScreenQuad> ScreenQuadPtr;

class ScreenQuad : public Quad2DGL {
public:
  static ScreenQuadPtr Create() {
    return ScreenQuadPtr(new ScreenQuad());
  }
  const Matrix4x4 &mvp() const { return mvp_; }
  const float *mvp_ptr() const { return &mvp_[0][0]; }
  void Draw(ax::ProgramGLSLPtr prog);
  using Quad2DGL::Draw;
private:
  ScreenQuad();
private:
  Matrix4x4 mvp_;
};

inline void DrawCube(const Point &p1, const Point &p2) {
  DrawCube(p1.x, p1.y, p1.z, p2.x, p2.y, p2.z);
}

class DEPRECATED(MyScreenQuad) {
public:
  bool Initialize();
  void Draw();
private:
  uint32 vbo_;
  int vertex_location_;

  static const float quadVArray[24];
};

} // ax

#endif // AXLE_CG_CG_GL_H