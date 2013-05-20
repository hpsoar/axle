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

void DrawQuad(float x1, float y1, float x2, float y2) {
  glBegin(GL_QUADS);
  glTexCoord2f(x1, y1); glVertex2f(x1, y1);
  glTexCoord2f(x1, y2); glVertex2f(x1, y2);
  glTexCoord2f(x2, y2); glVertex2f(x2, y2);
  glTexCoord2f(x2, y1); glVertex2f(x2, y1);
  glEnd();
}

void DrawQuad(float x1, float y1, float z1,  float x2, float y2, float z2) {
  glBegin(GL_QUADS);
  glEnd();
}

void DrawQuad(float x1, float y1 ,float x2, float y2, float z) {
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(x1, y1, z);
  glTexCoord2f(0, 1); glVertex3f(x1, y2, z);
  glTexCoord2f(1, 1); glVertex3f(x2, y2, z);
  glTexCoord2f(1, 0); glVertex3f(x2, y1, z);
  glEnd();
}

void DrawCube(float size);

void DrawCube(float x1, float y1, float z1, float x2, float y2, float z2) {
  glBegin(GL_QUADS);

  glNormal3f(0, -1, 0); 
  glVertex3f(x1, y1, z1);
  glVertex3f(x2, y1, z1);
  glVertex3f(x2, y1, z2);
  glVertex3f(x1, y1, z2);

  glNormal3f(0, 1, 0); 
  glVertex3f(x1, y2, z1);
  glVertex3f(x1, y2, z2);
  glVertex3f(x2, y2, z2);
  glVertex3f(x2, y2, z1);

  glNormal3f(-1, 0, 0); 
  glVertex3f(x1, y1, z1);
  glVertex3f(x1, y1, z2);
  glVertex3f(x1, y2, z2);
  glVertex3f(x1, y2, z1);

  glNormal3f(1,0, 0); 
  glVertex3f(x2, y1, z1);
  glVertex3f(x2, y2, z1);
  glVertex3f(x2, y2, z2);
  glVertex3f(x2, y1, z2);

  glNormal3f(0, 0, -1); 
  glVertex3f(x1, y1, z1);
  glVertex3f(x1, y2, z1);
  glVertex3f(x2, y2, z1);
  glVertex3f(x2, y1, z1);

  glNormal3f(0, 0, 1);
  glVertex3f(x1, y1, z2);
  glVertex3f(x2, y1, z2);
  glVertex3f(x2, y2, z2);
  glVertex3f(x1, y2, z2);
  glEnd();
}

GLuint MakeFullScreenQuad() {
  GLuint display_list = glGenLists(1);
  if (display_list) {
    glNewList(display_list, GL_COMPILE);
    
    // the vertex shader only need to pass the vertex through
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    //glRectf(-1, -1, 1, 1);
    DrawQuad(0, 0, 1, 1);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEndList();
  }
  return display_list;
}

//// MyScreenQuad

bool MyScreenQuad::Initialize() {
  glGenBuffers(1, &this->vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(this->quadVArray), 
               this->quadVArray, GL_STATIC_DRAW);
  
  this->vertex_location_ = 0;
  return true;
}

void MyScreenQuad::Draw() {
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo_);
  glEnableVertexAttribArray(this->vertex_location_);
  glVertexAttribPointer(this->vertex_location_, 4, GL_FLOAT, GL_FALSE, 
                        sizeof(GLfloat) * 4, 0);
  glDrawArrays(GL_TRIANGLES, 0, 24);
  ax::CheckErrorsGL("MyScreenQuad::Draw");
}

const GLfloat MyScreenQuad::quadVArray[] = {
   -1.0f, -1.0f, 0.0f, 1.0f,
   1.0f, -1.0f, 0.0f, 1.0f,
   -1.0f, 1.0f, 0.0f, 1.0f,    
   1.0f, -1.0f, 0.0f, 1.0f,
   1.0f, 1.0f, 0.0f, 1.0f,
   -1.0f, 1.0f, 0.0f, 1.0f  
};

} // ax
