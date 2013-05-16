#ifndef AX_CG_PARAMS_H
#define AX_CG_PARAMS_H

#include <map>

#include "../geom.h"
#include "../cg/texture_gl.h"

namespace ax {
template<typename T>
struct __ParamSet {
public:
  typedef T KeyType;

  int GetInt(const KeyType &name, int d=0) const { 
    return FIND(this->ints_, name, d);
  }
  uint64 GetUint64(const KeyType &name, uint64 d=0) const {
    return FIND(this->uint64s_, name, d);    
  }
  bool GetBool(const KeyType &name, bool d=false) const { 
    return FIND(this->bools_, name, d);    
  }
  float GetFloat(const KeyType &name, float d=0.f) const {
    return FIND(this->floats_, name, d);    
  }
  const KeyType GetStr(const KeyType &name, const std::string &d="") const {
    return FIND(this->strs_, name, d);    
  }
  void SetInt(const KeyType &name, int value) {
    this->ints_[name] = value;
  }
  void SetUint64(const KeyType &name, uint64 value) {
    this->uint64s_[name] = value;
  }
  void SetBool(const KeyType &name, bool value) {
    this->bools_[name] = value;
  }
  void SetFloat(const KeyType &name, float value) {
    this->floats_[name] = value;
  }
  void SetStr(const KeyType &name, const std::string &value) {
    this->strs_[name] = value;
  }
protected:
  template<typename CT>
  static typename CT::mapped_type FIND(const CT &m, const typename CT::key_type &key, typename CT::mapped_type d) {
    typename CT::const_iterator it = m.find(key);
    if (it != m.end()) return it->second;
    return d;    
  }
private:
  typedef std::map<KeyType, int> IntParams;
  typedef std::map<KeyType, bool> BoolParams;
  typedef std::map<KeyType, float> FloatParams;
  typedef std::map<KeyType, std::string> StrParams;
  typedef std::map<KeyType, uint64> Uint64Params;

  IntParams ints_;
  BoolParams bools_;
  StrParams strs_;
  FloatParams floats_;
  Uint64Params uint64s_;
};

template<typename T>
struct __ParamSet2 : public __ParamSet<T> {
public:  
  typedef typename __ParamSet<T>::KeyType KeyType;
  ax::Matrix4x4 GetMat4(const KeyType &name, 
                        const ax::Matrix4x4 &d=ax::Matrix4x4(1.f)) const {
    return FIND(this->mat4s_, name, d);
  }
  ax::Vector3 GetVec3(const KeyType &name, 
                      const ax::Vector3 &d=ax::Vector3(0)) const {
    return FIND(this->vec3s_, name, d);
  }
  template<typename PTR>
  PTR GetPtr(const KeyType &name) const {
    return (PTR)FIND(this->ptrs_, name, NULL);
  }
  ax::Texture2DPtr GetTexture(const KeyType &name) const {
    return FIND(this->textures_, name, NULL);
  }

  void SetMat4(const KeyType &name, const ax::Matrix4x4 &m) {
    this->mat4s_[name] = m;
  }
  void SetVec3(const KeyType &name, const ax::Vector3 &v) {
    this->vec3s_[name] = v;
  }
  template<typename PTR>
  void SetPtr(const KeyType &name, PTR ptr) {
    this->ptrs_[name] = (void*)ptr;
  }
  void SetTexture(const KeyType &name, ax::Texture2DPtr texture) {
    this->textures_[name] = texture;
  }
private:
  typedef std::map<KeyType, ax::Matrix4x4> Mat4Params;
  typedef std::map<KeyType, ax::Vector3> Vec3Params;
  typedef std::map<KeyType, void*> PtrParams;
  typedef std::map<KeyType, ax::Texture2DPtr> TexParams;

  Mat4Params mat4s_;
  Vec3Params vec3s_;
  PtrParams ptrs_;
  TexParams textures_;
};

typedef __ParamSet<std::string> ParamSet;
typedef __ParamSet2<std::string> ParamSet2;

}

#endif // AX_CG_PARAMS_H

