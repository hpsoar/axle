#ifndef AXLE_CG_TEXTURE_GL_H
#define AXLE_CG_TEXTURE_GL_H

#include "../core.h"

#include "image.h"
#include "cube_image.h"

#include "cg_fwd.h"
#include "gl_object.h"

namespace ax {
class UnitIdManager {
public:
  int nextId() {
    int id = this->next_id_++;
    if (this->next_id_ > this->kMaxUnit) {
      ax::Logger::Log("More than %d image units are used, you may need more efficient management", this->kMaxUnit);
    }
    return id;
  }
private:
  static int next_id_;
  static const int kMaxUnit = 16;
};

class TextureGL : public GLObject {
public:
  TextureGL(uint32 target, const std::string &name) 
      : GLObject(name), target_(target),
        width_(0), height_(0), iformat_(0), unit_id_(-1) {
    glGenTextures(1, &id_);
  }

  virtual ~TextureGL() {
    if (id_ > 0) glDeleteTextures(1, &id_);
    printf("release: %s\n", this->name().c_str());
  }

  void Enable() const { glEnable(this->target()); }
  void Disable() const { glDisable(this->target()); }
  void Bind() const { glBindTexture(this->target(), this->id()); }
  void Unbind() const { glBindTexture(this->target(), 0); }

  void BindImageTexture(uint32 unit, int32 level, 
                        bool layered, int32 layer, 
                        uint32 access, uint32 format) {
    glBindImageTexture(unit, this->id(), level,
                       layered, layer, access, format);
    this->unit_id_ = unit;
    this->CheckResult("TextureGL::BindImageTexture");
  }

  void BindImageTexture(int32 level, bool layered, int32 layer, uint32 access, uint32 format) {
    this->BindImageTexture(this->unit_mgr_.nextId(), level, layered, layer, access, format);
  }

  int ImageUnit() const { 
    assert(this->unit_id_ >= 0);
    if (this->unit_id_ < 0) ax::Logger::Log("call BindImageTexture first");
    return this->unit_id_; 
  }

  void SetDefaultParameters();

  void SetParameter(uint32 name, int32 val) {
    glTexParameteri(target_, name, val);
  }
  void SetParameter(uint32 name, int32 *val) {
    glTexParameteriv(target_, name, val);
  }
  void SetParameter(uint32 name, float val) {
    glTexParameterf(target_, name, val);
  }
  void SetParameter(uint32 name, float *val) {
    glTexParameterfv(target_, name, val);
  }
  
  uint32 target() const { return target_; }

  bool is_valid() const { return 0 != this->id(); }

  int width() const { return width_; }
  int height() const { return height_; }
  int iformat() const { return iformat_; }
protected:
  void Set(int w, int h, int iformat) {
    width_ = w; 
    height_ = h; 
    iformat_ = iformat;
  }

private:  
  uint32 target_;
  
  int width_;
  int height_;
  int iformat_;

  int unit_id_;

  static ax::UnitIdManager unit_mgr_;

  DISABLE_COPY_AND_ASSIGN(TextureGL);
};

class CubeTexture : public TextureGL {
public:
  CubeTexture(const std::string &name = "") : 
      TextureGL(GL_TEXTURE_CUBE_MAP, name) { }

  bool Initialize(int w, int h, int i_format);
  bool Initialize(const CubeImage &image, int i_format);
  bool Initialize(const Image *images[6], int i_format);
};

class Texture2D : public TextureGL {
public:
  static Texture2DPtr Create(int target, const std::string &name = "") {
    return Texture2DPtr(new Texture2D(target, name));
  }

  static Texture2DPtr Create2D(const std::string &name = "") {
    return Create(GL_TEXTURE_2D, name);
  }

  static Texture2DPtr CreateRect(const std::string &name = "") {
    return Create(GL_TEXTURE_RECTANGLE, name);
  }

  bool Initialize(int w, int h, int i_format);
  bool Initialize(const Image &image, int i_format);
  bool InitializeDepth(int w, int h, int i_format);

  void GenerateMipmap() { 
    this->Bind();
    glGenerateMipmap(this->target()); 
  }  
  
  void Save(const char *filename, int level = 0) const;
  void Save(const char *filename, int level, int format, int type,
            void *data) const;

  ImagePtr GetTextureImage(int level, int format=GL_RGBA, 
                           int type=GL_UNSIGNED_BYTE, 
                           int n_channels=4, int depth=1) const;
private:
  Texture2D(uint32 target, const std::string &name) : 
      TextureGL(target, name) { } 
};

class Texture3D : public TextureGL {
public:
  static Texture3DPtr Create(int target, const std::string &name = "") {
    return Texture3DPtr(new Texture3D(target, name));
  }  

  bool Initialize(int w, int h, int z, int i_format);
  void Save(const char *filename, int z, int level = 0) const;   
  int depth() const { return depth_; }
private:
  Texture3D(int target, const std::string &name) 
      : TextureGL(target, name), depth_(0) { }
   
  int depth_;  
};

} // ax

#endif // AXLE_CG_TEXTURE_GL_H

