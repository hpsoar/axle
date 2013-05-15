#include "axle/cg/utils.h"

#if defined(SYS_IS_APPLE)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdio.h>

#include "axle/core/debug.h"
#include "axle/cg/texture_gl.h"
#include "axle/cg/image.h"

namespace ax {
GLuint64 GetGPUPtr(uint32 id, uint32 access) {
  glBindBuffer(GL_ARRAY_BUFFER, id);

  GLuint64 gpu_ptr = 0;
  glMakeBufferResidentNV(GL_ARRAY_BUFFER, access);
  glGetBufferParameterui64vNV(GL_ARRAY_BUFFER, GL_BUFFER_GPU_ADDRESS_NV,
                              &gpu_ptr);

  glBindBuffer(GL_ARRAY_BUFFER, 0); 

  return gpu_ptr;
}

bool ArrayBufferGL::Initialize(uint32 size, uint32 access, const void *data) {
  V_RET(size != 0);
  if (this->size_ != size) {
    this->Release();
    glGenBuffers(1, &this->id_);
    glBindBuffer(GL_ARRAY_BUFFER, this->id_);

    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STREAM_DRAW);
    V_RET(!ax::CheckErrorsGL("ArrayBufferGL::Initialize"));
    this->size_ = size;
    glMakeBufferResidentNV(GL_ARRAY_BUFFER, access);
    glGetBufferParameterui64vNV(GL_ARRAY_BUFFER, GL_BUFFER_GPU_ADDRESS_NV,
                                &this->gpu_ptr_);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }  
  return true;
}

void ArrayBufferGL::Release() {
  if (this->id_ > 0) {
    glDeleteBuffers(1, &this->id_);
    id_ = 0;
    gpu_ptr_ = 0;
    size_ = 0;
  }
}

void ArrayBufferGL::SetData(int offset, int size, const void *data) {
  glBindBuffer(GL_ARRAY_BUFFER, this->id_);
  glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);  

  glBindBuffer(GL_ARRAY_BUFFER, 0);
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

void DisplayStatistics(const char *name, float data, const char *unit, 
                       int x, int y) {
  char buff[256];
  sprintf(buff, "%s : %.3f %s", name, data, unit);
  glColor3f(0, 0, 0);
  DrawTextGL(x, y, buff);
  glColor3f(1, 1, 1);  
  DrawTextGL(x+1, y+1, buff);
}

static const char *vis_rect_tex_fp = 
  "#version 330"
  "uniform sampler2DRect g_tex;"
  "out vec4 g_color;"
  "void main() "
  "{"
  "   ivec2 tcoord = ivec2(gl_FragCoord.xy)"
  "   g_color = sampler2DRect(g_tex, tcoord)"
  "}";
static const char *vis_tex_fp =
  "#version 330"
  "uniform sampler2D g_tex;"
  "out vec4 g_color;"
  "uniform float g_width;"
  "uniform float g_height;"
  "void main() "
  "{"
  "   vec2 tcoord = vec2(gl_FragCoord.x/g_width, gl_FragCoord.y/g_height);"
  "   g_color = sampler2D(g_tex, tcoord)"
  "}";

static const char *vis_tex_vp =
  "layout (location = 0) in vec3 position;"
  ""
  "";

void VisualizeTexture(const Texture2DPtr tex) {  
  if (NULL == tex) return;
  glViewport(0, 0, tex->width(), tex->height());
  glPushAttrib(GL_ALL_ATTRIB_BITS);  
  glDepthMask(GL_FALSE);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, 1, 0, 1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_TEXTURE);
  glPushMatrix();
  glLoadIdentity();
  
  tex->Enable();
  tex->Bind();

  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  glTexParameteri(tex->target(), GL_TEXTURE_COMPARE_MODE, GL_NONE);
  // Show the shadowMap at its actual size relative to window

  // the following applys only to 2d texture, for rectangle texture
  // the tex quad should be corresponding to texture size
  glColor3f(0, 0, 0);
  DrawQuad(0.0, 0.0, 1.0, 1.0);  

  tex->Disable();

  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glPopAttrib();
}

bool CheckErrorsGL(const char* location) {	
  GLuint error_id;
  const char *error_str;
  bool error_occurred = false;
  while (error_id = glGetError()) {
    error_str = reinterpret_cast<const char*>(gluErrorString(error_id));
    Logger::Log("In \"%s\": error: %u, %s", location, error_id, error_str);
    error_occurred = true;
  }
  return error_occurred;
}

bool CheckGLInfo() {
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    Logger::Log("glewInit", glewGetErrorString(err));
    return false;
  }
  
  Logger::Log("VENDOR: %s", glGetString(GL_VENDOR));
  Logger::Log("RENDERER: %s", glGetString(GL_RENDERER));
  Logger::Log("VERSION: %s", glGetString(GL_VERSION));
  Logger::Log("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));

  Logger::Log("");

  GLint max_draw_buffers;
  glGetIntegerv(GL_MAX_DRAW_BUFFERS, &max_draw_buffers);
  Logger::Log("Max Draw Buffers: %d", max_draw_buffers); 

  GLint max_texture_coords;
  glGetIntegerv(GL_MAX_TEXTURE_COORDS, &max_texture_coords);
  Logger::Log("Max Textures Coords: %d", max_texture_coords);

  GLint max_textures;
  glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS , &max_textures);
  Logger::Log("Max Textures: %d", max_textures);

  GLint max_tex_size;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_tex_size);
  Logger::Log("Max Texture Size: %d", max_tex_size);
  
  if (!glewIsSupported("GL_EXT_gpu_shader4")) {
    Logger::Log("GL_EXT_gpu_shader4 unsupported");
  }
  if (!glewIsSupported("GL_EXT_texture_integer")) {
    Logger::Log("GL_EXT_texture_integer unsupported");
  }
  if (!glewIsSupported("GL_ARB_color_buffer_float")) {
    Logger::Log("GL_EXT_texture_integer unsupported"); 
  }
  return true;
}

void SaveFrameBuffer(const char *filename) {
  const int w = glutGet(GLUT_WINDOW_WIDTH);
  const int h = glutGet(GLUT_WINDOW_HEIGHT);
  float *pixels = new float[w * h * 4];
  glReadPixels(0, 0, w, h, GL_RGBA, GL_FLOAT, pixels);
  SaveImage(filename, w, h, 1, 4, GL_RGBA, GL_FLOAT, pixels);
  delete[] pixels;
}

} // ax
