#include "../cg/scene.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include "../cg/object-factory.h"
#include "../cg/material-factory.h"
#include "../cg/texture_gl.h"
#include "../cg/group.h"
#include "../cg/parser.h"

namespace ax {
ScenePtr Scene::Create(const std::string &file, Options opts) {
  ScenePtr ptr = ScenePtr(new Scene);
  bool ret = ptr->Load(file);
  if (!ret) return ScenePtr();
  ptr->PreProcess(opts);
  return ptr;
}

Scene::~Scene() { }

bool Scene::Load(const std::string &filename) {
  FILE *fp = fopen(filename.c_str(), "r");
  if (NULL == fp) {
    ax::Logger::Log("Failed to open '%s'", filename.c_str());
    return false;
  }
  
  geometry_ = ax::ObjectFactory::CreateGroup();

  LineToken token;
  char buf[512];

  while (fgets(buf, 512, fp) != NULL) {
    const char *ptr = ax::StripLeading(buf);
    if (ax::IsBlankOrComment(ptr)) continue;

    ptr = token.Extract(ptr);
    if (token.Equals("material")) {
      this->LoadMaterial(ptr, fp);
    }
    else if (token.Equals("object")) {
      ObjectPtr obj = this->LoadObject(ptr, fp);
      geometry_->Add(obj);
    }
    else {
      ax::UnknownKeyword(token.ptr(), "Scene");
      ax::EatToEndKeyword(fp, token);
    }
  }
  fclose(fp);
  return true;
}

/*
 * @buff: material <@buff: matl_type [matl_name]>
 */
MaterialPtr Scene::LoadMaterial(const char *buff, FILE *fp) {
  LineToken token;
  const char *ptr = token.Extract(buff);
  MaterialPtr mtl;
  if (token.Equals("glslmtl")) {
    ptr = token.Extract(ptr);
    mtl = ax::MaterialFactory::CreateGLSLMaterial(token.ptr(), fp, this);
  }
  else if (token.Equals("glmtl")) {
    ptr = token.Extract(ptr);
    mtl = ax::MaterialFactory::CreateGLMaterial(token.ptr(), fp);
  }
  else {
    ax::Unsupported("material type", token.ptr());    
    ax::EatToEndKeyword(fp, token);
  }
  
  this->Add(token.ptr(), mtl);

  return mtl;
}

/*
 * @buff: the string after token 'object'
 */
ObjectPtr Scene::LoadObject(const char *buff, FILE *fp) {
  LineToken token;
  const char *ptr = token.Extract(buff);
  
  ObjectPtr obj;
  if (token.Equals("obj")) {   
    // ignore groups in wavefront obj file
    obj = ax::ObjectFactory::LoadWavefront(fp, this, kIgnoreGroup);   
    //ptr = token.Extract(ptr); // remove object type: hem/obj/...
  }
  else if (token.Equals("obj_g")) {
    obj = ax::ObjectFactory::LoadWavefront(fp, this, kNone);
  }
  else if (token.Equals("group") || token.Equals("instance")) {
    obj = ax::ObjectFactory::CreateGroup(fp, this);
  }
  else {
    ax::Unsupported("object type", token.ptr());    
    ax::EatToEndKeyword(fp, token);
  }
  ptr = token.Extract(ptr); // object name
  this->Add(token.ptr(), obj);
    
  return obj;
}

Texture2DPtr Scene::LoadTexture(const char *buff) {
  Texture2DPtr tex = this->texture(buff);
  if (tex != NULL) return tex;

  const std::string &path = ax::UserOptions::GetFullTexturePath(buff);
  tex = this->texture(path);
  if (tex != NULL) return tex;
  
  ax::Image img;
  if (!img.Load(path.c_str())) return Texture2DPtr();
  tex = ax::Texture2D::Create2D(buff);
  if (tex != NULL && tex->Initialize(img, GL_RGBA)) {
    tex->SetDefaultParameters();
    tex->SetParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
    tex->SetParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
    this->Add(path, tex);
  }
  return tex;
}

void Scene::Draw(Options opts) const {  
  geometry_->Draw(this, opts);
}

void Scene::Draw(ProgramGLSLPtr prog, Options opts) const {
  geometry_->Draw(prog, opts);
}

void Scene::PreProcess(Options opts) {
  geometry_->Transform();  
  geometry_->PreProcess(opts);
  bound_ = geometry_->bound();
}

void Scene::DepthRange(const Matrix4x4 &mat, 
                       float *min_depth, float *max_depth) const {
  ax::AABB box = geometry_->ComputeBound(mat);
  *min_depth = box.minP.z;
  *max_depth = box.maxP.z;
}
} // ax
