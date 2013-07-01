#ifndef AXLE_CG_SCENE_H
#define AXLE_CG_SCENE_H

#include "../core.h"
#include "../geom.h"
#include "../cg/cg_fwd.h"
#include "../cg/group.h"
#include <map>

namespace ax {
typedef std::map<std::string, int> IntVarMap;
typedef std::map<std::string, float> FloatVarMap;
class NamedVariables {
public:
  bool Load(const std::string &filename);
  int get_int(const std::string &name) { return ints_[name]; }
  float get_float(const std::string &name) { return floats_[name]; }
private:
  IntVarMap ints_;
  FloatVarMap floats_;
};

typedef std::map<std::string, ax::ObjectPtr> ObjectMap;
typedef std::map<std::string, ax::MaterialPtr> MaterialMap;
typedef std::map<std::string, ax::Texture2DPtr> TextureMap;

class Scene {
public:
  static ScenePtr Create(const std::string &file, Options opts);

  ~Scene();  

  void Draw(Options opts) const;
  void Draw(ProgramGLSLPtr prog, Options opts) const;
  
  void DepthRange(const Matrix4x4 &mat, 
                  float *min_depth, float *max_depth) const;
  const AABB &bound() const { return bound_; }

  MaterialPtr material(const std::string &name) { 
    return this->named_matls_[name]; 
  }
  ObjectPtr named_object(const std::string &name) { 
    return this->named_objs_[name]; 
  }
 ObjectPtr object(int i) const { return this->geometry_->object(i); }

  Texture2DPtr texture(const std::string &name) { 
    return this->named_texs_[name]; 
  }

  uint32 n_primitives() const { return this->geometry_->n_primitives(); }

  uint32 n_vertices() const { return this->geometry_->n_vertices(); }

  void GetVertices(ConstVertexSet &vertexet) const { 
    this->geometry_->GetVertices(vertexet); 
  }

  void GetIndices(ConstIndexSet &indexset) const { 
    this->geometry_->GetIndices(indexset);  
  }
private:
  void Add(const std::string &name, ObjectPtr obj) {
    if (obj == NULL || name.empty()) return;
    this->named_objs_[name] = obj;
  }

  void Add(const std::string &name, MaterialPtr matl) {
    if (matl == NULL || name.empty()) return;
    this->named_matls_[name] = matl;
  }

  void Add(const std::string &name, Texture2DPtr tex) {
    if (tex == NULL || name.empty()) return;
    this->named_texs_[name] = tex;
  }

  Scene(ax::Options opts) : opts_(opts) { }
  bool Load(const std::string &filename);
  MaterialPtr LoadMaterial(const char *type, FILE *fp);
  ObjectPtr LoadObject(const char *type, FILE *fp);
  Texture2DPtr LoadTexture(const char *ptr);

  //NOTE: some operation must be processed after all scene data loaded, should rename it PostProcess
  //      such as transform, a group can transform only after it's sub objects loaded
  //      bound can be computed only when objects are loaded and transformed
  //      only the transformed mesh can be loaded to vbo, otherwise the data is not correct
  void PreProcess();
private:
  ax::GroupPtr geometry_;
  AABB bound_;

  ObjectMap named_objs_;
  MaterialMap named_matls_;
  TextureMap named_texs_;

  ax::Options opts_;

  DISABLE_COPY_AND_ASSIGN(Scene);
  friend class ObjectFactory;
  friend class MaterialFactory;
};

} // ax
#endif // AXLE_CG_SCENE_H

