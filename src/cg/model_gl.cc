#include "model_gl.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include "utils.h"
#include "program_glsl.h"

namespace ax {
const uint32 Quad2DGL::indices_[6] = { 0, 1, 2, 0, 2, 3 };

void Quad2DGL::Draw() {
  glEnableVertexAttribArray(vertex_slot_);
  glEnableVertexAttribArray(tcoord_slot_);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx_vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vert_vbo_);

  glVertexAttribPointer(vertex_slot_, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glVertexAttribPointer(tcoord_slot_, 2, GL_FLOAT, GL_FALSE, 0, 
                        (GLvoid*)sizeof(vertices_));

  // the data type must be right
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  glDisableVertexAttribArray(vertex_slot_);
  glDisableVertexAttribArray(tcoord_slot_);
  CheckErrorsGL("Quad2DGL::Draw");
}

void Quad2DGL::CreateVBO() {
  glGenBuffers(1, &vert_vbo_);
  glGenBuffers(1, &idx_vbo_);
  
  const int vert_size = sizeof(vertices_);
  const int tcoord_size = sizeof(tcoords_);
  glBindBuffer(GL_ARRAY_BUFFER, vert_vbo_);
  glBufferData(GL_ARRAY_BUFFER, vert_size + tcoord_size, NULL,
               GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, vert_size, vertices_);
  glBufferSubData(GL_ARRAY_BUFFER, vert_size, tcoord_size, tcoords_); 

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx_vbo_);  
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_), indices_, 
               GL_STATIC_DRAW); 
  CheckErrorsGL("Quad2DGL::CreateVBO");
}

ScreenQuad::ScreenQuad() : Quad2DGL(0, 0, 1, 1) {
  mvp_ = glm::ortho(0, 1, 0, 1);
}

void ScreenQuad::Draw(ax::ProgramGLSLPtr prog) {
  prog->Set4DMatVar("g_mvp_mat", this->mvp());
  this->Draw();
}

} // ax
