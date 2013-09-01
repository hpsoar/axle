#include "utils.h"

#if defined(SYS_IS_APPLE)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdio.h>

#include "../core/debug.h"
#include "texture_gl.h"
#include "image.h"
#include "program_glsl.h"
#include "render_device_fbo.h"

namespace ax {
//// TextureCopier

void TextureCopier::Copy(ax::Texture2DPtr texture) {
  TextureCopier::Copy(texture, texture->width(), texture->height());
}

void TextureCopier::Copy(ax::Texture2DPtr texture, int width, int height) {
  glViewport(0, 0, width, height);  
  if (texture->target() == GL_TEXTURE_2D) {
    TextureCopier::CopyTexture2D(texture);
  }
  else if (texture->target() == GL_TEXTURE_RECTANGLE) {
    TextureCopier::CopyTextureRect(texture);
  }  
}

const char *quad_src = 
"#version 330 \n"
"layout (location = 0) in vec2 i_position; \n"
"layout (location = 1) in vec2 i_tcoord; \n"
"uniform mat4 g_mvp_mat; \n"
"out vec2 g_tcoord; \n"
"void main(void) { \n"
"  gl_Position = g_mvp_mat * vec4(i_position, 0, 1.0); \n"
"  g_tcoord = i_tcoord; \n"
"}\n";

const char *copy_tex_rect_src =
"#version 330 \n"
"#extension GL_ARB_texture_rectangle : enable \n"
"out vec4 g_color; \n"
"uniform sampler2DRect g_tex;\n"
"void main() {\n"
"  ivec2 tcoord = ivec2(gl_FragCoord.xy);\n"
"  g_color = texture2DRect(g_tex, tcoord);\n"
"}\n" ;

const char *copy_tex_2d_src = 
"#version 330\n"
"out vec4 g_color;\n"
"uniform sampler2D g_tex;\n"
"uniform int g_width;\n"
"uniform int g_height;\n"
"void main() {\n"
"  vec2 tcoord = vec2(gl_FragCoord.x / g_width, gl_FragCoord.y / g_height);\n"
"  g_color = texture2D(g_tex, tcoord);\n"
"}\n";

const char *min_max_mipmap_src = 
"#version 400\n"
"layout (location = 0) out vec4 out_color;\n"
"uniform sampler2D g_input_tex;\n"
"uniform float g_offset;\n"
"in vec2 g_tcoord;\n"
"vec2 MinMaxDepth(float xoffset, float yoffset) {\n"
"  return texture2D(g_input_tex, g_tcoord + vec2(xoffset, yoffset)).xy;\n"
"}\n"
"void main() {\n"
"  vec2 mmd0 = MinMaxDepth(0, 0);\n"
"  vec2 mmd1 = MinMaxDepth(g_offset, 0);\n"
"  vec2 mmd2 = MinMaxDepth(g_offset, g_offset);\n"
"  vec2 mmd3 = MinMaxDepth(0, g_offset);\n"
"  vec2 mmd = mmd0;\n"
"  mmd.x = min(mmd.x, mmd1.x);\n"
"  mmd.y = max(mmd.y, mmd1.y);\n"
"  mmd.x = min(mmd.x, mmd2.x);\n"
"  mmd.y = max(mmd.y, mmd2.y);\n"
"  mmd.x = min(mmd.x, mmd3.x);\n"
"  mmd.y = max(mmd.y, mmd3.y);\n"
"  out_color.xy = mmd;\n"
"}\n";

// need shaders, TODO: solve the dependency
void TextureCopier::CopyTexture2D(ax::Texture2DPtr texture) {
  static ax::ProgramGLSLPtr prog = NULL;
  if (prog == NULL) {
    RET(prog = ax::ProgramGLSL::CreateFromCode(quad_src, copy_tex_2d_src, "copy_tex_2d program"));
    if (!prog->Link()) prog = NULL;
  }
  RET(prog);

  prog->Begin();
  prog->SetTextureVar("g_tex", texture);
  prog->SetVar("g_width", texture->width());
  prog->SetVar("g_height", texture->height());
  TextureCopier::DrawFullScreenQuad(prog);
  prog->End();
}

// TODO: solve dependency on shaders
void TextureCopier::CopyTextureRect(ax::Texture2DPtr texture) {
  static ax::ProgramGLSLPtr prog = NULL;
  if (prog == NULL) {
    RET(prog = ax::ProgramGLSL::CreateFromCode(quad_src, copy_tex_rect_src, "copy_tex_2d program"));
    if (!prog->Link()) prog = NULL;
  }
  RET(prog);

  prog->Begin();
  prog->SetTextureVar("g_tex", texture);  
  TextureCopier::DrawFullScreenQuad(prog);
  prog->End();
}

void TextureCopier::DrawFullScreenQuad(ax::ProgramGLSLPtr prog) {
  static ax::ScreenQuadPtr quad = NULL;
  if (quad == NULL) quad = ax::ScreenQuad::Create();
  RET(quad);
  quad->Draw(prog);
}

// TextureUtil

// need corresponding shaders under shaders directory, TODO: solve this dependency
bool TextureUtil::Initialize() {
  V_RET(this->device_ = ax::RenderDeviceFBO::Create());
  V_RET(this->quad_ = ax::ScreenQuad::Create());

  V_RET(this->max_depth_derivative_prog_ = ax::ProgramGLSL::Create(
      "shaders/quad.vp", "shaders/max_depth_derivative.fp", 
      "max depth derivative"));
  V_RET(this->max_depth_derivative_prog_->Link());

  V_RET(this->min_max_normal_prog_ = ax::ProgramGLSL::Create(
      "shaders/quad.vp", "shaders/min_max_normal.fp", "min max normal"));
  V_RET(this->min_max_normal_prog_->Link());

 /* V_RET(this->reduction_prog_ = ax::ProgramGLSL::Create(
      "shaders/quad.vp", "shaders/min_max_depth_mipmap.fp", "depth mipmap"));
  V_RET(this->reduction_prog_->Link());*/

  return true;
}

ax::ScreenQuadPtr ax::TextureUtil::quad_;
ax::FBODevicePtr ax::TextureUtil::device_;

bool TextureUtil::Initiaialze() {
  if (TextureUtil::device_ == NULL) {
    V_RET(TextureUtil::device_ = ax::RenderDeviceFBO::Create());
  }
  if (TextureUtil::quad_ == NULL) {
    V_RET(TextureUtil::quad_ = ax::ScreenQuad::Create());
  }
  return true;
}

int TextureUtil::CreateCustomMipmap(ax::ProgramGLSLPtr shader, ax::Texture2DPtr texture, int min_res) {
  if (!TextureUtil::Initiaialze()) return 0;

  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_DEPTH_TEST);

  TextureUtil::device_->Activate();
  TextureUtil::device_->SaveMVP();

  shader->Begin();
  shader->Set4DMatVar("g_mvp_mat", TextureUtil::quad_->mvp());
  shader->SetTextureVar("g_input_tex", texture);
  
  float step = 1.0f;
  int width = texture->width() / 2;
  int level = 1;
  
  texture->Bind();  

  while (width >= min_res) {
    TextureUtil::device_->SetRenderTarget(texture, level);
    TextureUtil::device_->AdjustViewport(width, width);   
    glClear(GL_COLOR_BUFFER_BIT);
    ax::CheckErrorsGL("xxx");

    texture->SetParameter(GL_TEXTURE_BASE_LEVEL, level - 1);
    texture->SetParameter(GL_TEXTURE_MAX_LEVEL, level - 1);
    texture->SetParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    texture->SetParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    shader->SetVar("g_offset", step / texture->width());

    ax::CheckErrorsGL("hello");
    TextureUtil::quad_->Draw();

    ++level;
    step *= 2;
    width /= 2;
  }
  texture->SetParameter(GL_TEXTURE_BASE_LEVEL, 0);  
  texture->SetParameter(GL_TEXTURE_MAX_LEVEL, level - 1);
  texture->Unbind();

  shader->End();

  TextureUtil::device_->RestoreMVP();
  TextureUtil::device_->Deactivate();

  glPopAttrib();

  if (ax::CheckErrorsGL("TextureUtil::CreateCustomMipmap")) return 0;

  return level;
}

void TextureUtil::CreateMaxDepthDerivativeTexture(
    ax::Texture2DPtr position_tex, ax::Texture2DPtr texture) {
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_DEPTH_TEST);

  this->device_->Activate();
  this->device_->SetRenderTarget(texture);
  this->device_->DisableDepthBuffer();
  this->device_->AdjustViewport(texture);
  glClear(GL_COLOR_BUFFER_BIT);

  this->max_depth_derivative_prog_->Begin();

  this->max_depth_derivative_prog_->Set4DMatVar("g_mvp_mat", this->quad_->mvp());
  this->max_depth_derivative_prog_->SetTextureVar(
      "g_position_buffer", position_tex);
  
  //this->max_depth_derivative_prog_->SetVar("g_zfar", this->camera_.z_far());

  this->max_depth_derivative_prog_->SetVar(
      "g_offset", 1.0f / texture->width());

  this->quad_->Draw(); 
  this->max_depth_derivative_prog_->End();

  this->device_->Deactivate();

  glPopAttrib();
  ax::CheckErrorsGL("TextureUtil::CreateMaxDepthDerivativeTexture");
}

void TextureUtil::CreateMinMaxNormalTexture(
    ax::Texture2DPtr normal_tex, ax::Texture2DPtr texture) {
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_DEPTH_TEST);

  this->device_->Activate();
  this->device_->SetRenderTarget(texture);
  this->device_->DisableDepthBuffer();
  this->device_->AdjustViewport(texture);
  glClear(GL_COLOR_BUFFER_BIT);

  this->min_max_normal_prog_->Begin();

  this->min_max_normal_prog_->Set4DMatVar("g_mvp_mat", this->quad_->mvp());
  this->min_max_normal_prog_->SetTextureVar("g_normal_tex", normal_tex);
  this->min_max_normal_prog_->SetVar("g_offset", 1.0f / texture->width());

  this->quad_->Draw();

  this->min_max_normal_prog_->End();

  this->device_->Deactivate();

  glPopAttrib();
}

void TextureUtil::ReduceMinMax(ax::ImagePtr img, float *ret) {
  RET(img != NULL && ret != NULL);

  const float *min_max = (const float*)(img->data());
  float dmin = FLT_MAX;
  float dmax = -dmin;
  for (int i = 0; i < img->width() * img->height(); ++i) {
    if (min_max[i*2] < dmin) dmin = min_max[i*2];
    if (min_max[i*2+1] > dmax) dmax = min_max[i*2+1];
  }
  ret[0] = dmin; ret[1] = dmax;
}

void TextureUtil::ReduceMinMax(ax::Texture2DPtr tex, float *ret) {
  static ax::ProgramGLSLPtr prog = NULL;
  if (prog == NULL) {
    RET(prog = ax::ProgramGLSL::CreateFromCode(quad_src, min_max_mipmap_src, "min max mipmap"));
    if (!prog->Link()) prog = NULL;
  }
  RET(prog);
  int levels = TextureUtil::CreateCustomMipmap(prog, tex, 64);

  TextureUtil::ReduceMinMax(tex->GetTextureImage(levels - 1, GL_RG, GL_FLOAT, 2, 4), ret);
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

void VisualizeTexture(const Texture2DPtr tex, bool full_screen) {  
  if (NULL == tex) return;
  if (full_screen) {
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_WIDTH);
    glViewport(0, 0, w, h);
  }
  else {
    glViewport(0, 0, tex->width(), tex->height());
  }  
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

bool GLContext::CheckGLInfo() {
  if (!GLContext::Initialize()) return false;

  Logger::Log("VENDOR: %s", glGetString(GL_VENDOR));
  Logger::Log("RENDERER: %s", glGetString(GL_RENDERER));
  Logger::Log("VERSION: %s", glGetString(GL_VERSION));
  Logger::Log("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));  

  Logger::Log("Max Draw Buffers: %d", max_draw_buffers); 
  Logger::Log("Max Textures Coords: %d", max_texture_coords);
  Logger::Log("Max Textures: %d", max_textures);
  Logger::Log("Max Image Units: %d", max_image_units);
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

bool GLContext::initialized = false;
GLint GLContext::max_draw_buffers = 0;
GLint GLContext::max_texture_coords = 0;
GLint GLContext::max_textures = 0;
GLint GLContext::max_image_units = 0;
GLint GLContext::max_tex_size = 0;

bool GLContext::Initialize() {
  if (GLContext::initialized) return true;

  GLenum err = glewInit();
  if (GLEW_OK != err) {
    Logger::Log("glewInit", glewGetErrorString(err));
    return false;
  }  
 
  glGetIntegerv(GL_MAX_DRAW_BUFFERS, &max_draw_buffers);  
  
  glGetIntegerv(GL_MAX_TEXTURE_COORDS, &max_texture_coords);  
  
  glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS , &max_textures);  
  
  glGetIntegerv(GL_MAX_IMAGE_UNITS, &max_image_units);  

  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_tex_size);  

  GLContext::initialized = true;

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
