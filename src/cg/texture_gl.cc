#include "texture_gl.h"
#include "utils.h"

namespace ax {
//int UnitIdManager::next_id_ = 0;
//const int UnitIdManager::kMaxUnit = GLContext::MaxImageUnits();
//
//UnitIdManager TextureGL::unit_mgr_;

void TextureGL::SetDefaultParameters() {
  this->Bind();
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  this->SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  this->SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  this->SetParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  this->SetParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

bool CubeTexture::Initialize(int w, int h, int i_format) {
  this->Bind();
  for (int i = 0; i < 6; ++i) {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, i_format,
                 w, h, 0, 0, 0, NULL);
  }
  return this->CheckResult("CubeTextue::Initialize");
}

bool CubeTexture::Initialize(const CubeImage &image, int i_format) {
  this->Bind();
  for (int i = 0; i < 6; ++i) {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, i_format, 
                 image.width(), image.height(), 0, image.format(),
                 image.type(), image.data(i));
  }
  return this->CheckResult("CubeTextue::Initialize");
}

bool CubeTexture::Initialize(const Image *images[6], int i_format) {
  this->Bind();
  for (int i = 0; i < 6; ++i) {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, i_format, 
                 images[i]->width(), images[i]->height(), 0, 
                 images[i]->format(), images[i]->type(), images[i]->data());
  }
  return this->CheckResult("CubeTextue::Initialize");
}

bool Texture2D::Initialize(int w, int h, int i_format) {
  int type = GL_FLOAT;
  int format = GL_RGBA;
  // TODO: the following is wrong, should pass in as parameters
  if ((i_format >= 0x8D70 && i_format < 0x8D9E) ||
      (i_format >= 0x8231 && i_format <= 0x823C)) {
    type = GL_UNSIGNED_INT;
    format = GL_RGBA_INTEGER;
  }
  this->Bind();
  glTexImage2D(this->target(), 0, i_format, w, h, 0, format, type, NULL);

  this->Set(w, h, i_format);
  return this->CheckResult("Texture2D::Initialize");
}

bool Texture2D::InitializeDepth(int w, int h, int i_format) {
  int type = GL_FLOAT;
  if (GL_DEPTH_COMPONENT == i_format) type = GL_UNSIGNED_BYTE;

  this->Bind();
  glTexImage2D(this->target(), 0, i_format, w, h, 0, GL_DEPTH_COMPONENT, type,
               NULL);

  this->Set(w, h, i_format);
  return this->CheckResult("Texture2D::InitializeDepth");
}

bool Texture2D::Initialize(const Image &image, int i_format) {
  this->Bind();  
  glTexImage2D(this->target(), 0, i_format, image.width(), image.height(), 0,
               image.format(), image.type(), image.data());

  this->Set(image.width(), image.height(), i_format);
  return this->CheckResult("Texture2D::Initialize");
}

ImagePtr Texture2D::GetTextureImage(int level, int format,  int type, 
                                    int n_channels, int depth) const {
  int width, height;
  this->Bind();
  glGetTexLevelParameteriv(this->target(), level, GL_TEXTURE_WIDTH, &width);
  glGetTexLevelParameteriv(this->target(), level, GL_TEXTURE_HEIGHT, &height);
  const int size = n_channels * width * height * depth; 

  const int iformat = this->iformat();
  char *data = new char[size];    
 /* if (iformat >= GL_R8I && iformat <= GL_R32UI) {
    format = GL_RED_INTEGER;    
  }*/
  glGetTexImage(this->target(), level, format, type, data);
  return Image::Create(width, height, format, type, data);
}

void Texture2D::Save(const char *filename, int level) const {
  ImagePtr image = this->GetTextureImage(level);
  SaveImage(filename, image->width(), image->height(), 1, 4, 
            image->format(), image->type(), image->data());  
  
  CheckErrorsGL("Texture2D::Save");
}

bool Texture3D::Initialize(int w, int h, int z, int i_format) {
  int type = GL_FLOAT;
  int format = GL_RGBA;
    // TODO: the following is wrong, should pass in as parameters
  if ((i_format >= 0x8D70 && i_format < 0x8D9E) ||
      (i_format >= 0x8231 && i_format <= 0x823C)) {
    type = GL_UNSIGNED_INT;
    format = GL_RGBA_INTEGER;
  }
  this->Bind();
  glTexImage3D(this->target(), 0, i_format, w, h, z, 0, format, type, NULL);

  this->Set(w, h, i_format);
  this->depth_ = z;  
  return this->CheckResult("Texture2DArray::Initialize");
}

void Texture3D::Save(const char *filename, int z, int level) const {
  int w, h;
  this->Bind();
  glGetTexLevelParameteriv(this->target(), level, GL_TEXTURE_WIDTH, &w);
  glGetTexLevelParameteriv(this->target(), level, GL_TEXTURE_WIDTH, &h);
}

} // ax
