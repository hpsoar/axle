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

// need shaders, TODO: solve the dependency
void TextureCopier::CopyTexture2D(ax::Texture2DPtr texture) {
  static ax::ProgramGLSLPtr prog = NULL;
  if (prog == NULL) {
    prog = ax::ProgramGLSL::Create(
        "shaders/pass_through.vp", "shaders/copy_tex_2d.fp",
        "copy_tex_2d program");
    prog->Link();
  }
  prog->Begin();
  prog->SetTextureVar("g_tex", texture);
  prog->SetVar("g_width", texture->width());
  prog->SetVar("g_height", texture->height());
  TextureCopier::DrawFullScreenQuad();
  prog->End();
}

// TODO: solve dependency on shaders
void TextureCopier::CopyTextureRect(ax::Texture2DPtr texture) {
  static ax::ProgramGLSLPtr prog = NULL;
  if (prog == NULL) {
    prog = ax::ProgramGLSL::Create(
        "shaders/pass_through.vp", "shaders/copy_tex_rect.fp",
        "copy_tex_2d program");
    prog->Link();
  }
  prog->Begin();
  prog->SetTextureVar("g_tex", texture);  
  TextureCopier::DrawFullScreenQuad();
  prog->End();
}

//// TextureUtil

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

  V_RET(this->reduction_prog_ = ax::ProgramGLSL::Create(
      "shaders/quad.vp", "shaders/min_max_depth_mipmap.fp", "depth mipmap"));
  V_RET(this->reduction_prog_->Link());

  return true;
}

int TextureUtil::CreateCustomMipmap(ax::ProgramGLSLPtr shader, 
                                    ax::Texture2DPtr texture,
                                    int min_res) {
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_DEPTH_TEST);

  this->device_->Activate();
  this->device_->SaveMVP();

  shader->Begin();
  shader->Set4DMatVar("mvp_mat", this->quad_->mvp());
  shader->SetTextureVar("g_input_tex", texture);
  
  float step = 1.0f;
  int width = texture->width() / 2;
  int level = 1;
  
  texture->Bind();  

  while (width >= min_res) {
    this->device_->SetRenderTarget(texture, level);
    this->device_->AdjustViewport(width, width);
    glClear(GL_COLOR_BUFFER_BIT);

    texture->SetParameter(GL_TEXTURE_BASE_LEVEL, level - 1);
    texture->SetParameter(GL_TEXTURE_MAX_LEVEL, level - 1);
    texture->SetParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    texture->SetParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);    

    shader->SetVar("g_offset", step / texture->width());

    this->quad_->Draw();

    ++level;
    step *= 2;
    width /= 2;
  }
  texture->SetParameter(GL_TEXTURE_BASE_LEVEL, 0);  
  texture->SetParameter(GL_TEXTURE_MAX_LEVEL, level - 1);
  texture->Unbind();

  shader->End();

  this->device_->RestoreMVP();
  this->device_->Deactivate();

  glPopAttrib();

  ax::CheckErrorsGL("TextureUtil::CreateCustomMipmap");

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

  this->max_depth_derivative_prog_->Set4DMatVar("mvp_mat", this->quad_->mvp());
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

  this->min_max_normal_prog_->Set4DMatVar("mvp_mat", this->quad_->mvp());
  this->min_max_normal_prog_->SetTextureVar("g_normal_tex", normal_tex);
  this->min_max_normal_prog_->SetVar("g_offset", 1.0f / texture->width());

  this->quad_->Draw();

  this->min_max_normal_prog_->End();

  this->device_->Deactivate();

  glPopAttrib();
}

// TODO: make it general
void TextureUtil::Reduce(ax::Texture2DPtr texture, float *ret, int n) { 
  int levels = this->CreateCustomMipmap(this->reduction_prog_, texture, 64); 

  ax::ImagePtr img = texture->GetTextureImage(
      levels - 1, GL_RG, GL_FLOAT, n, 4);

  const float *min_max = (const float*)(img->data());
  float dmin = FLT_MAX;
  float dmax = -dmin;
  for (int i = 0; i < img->width() * img->height(); ++i) {
    if (min_max[i*2] < dmin) dmin = min_max[i*2];
    if (min_max[i*2+1] > dmax) dmax = min_max[i*2+1];
  }
  ret[0] = dmin; ret[1] = dmax;
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
