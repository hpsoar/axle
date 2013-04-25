#ifndef AX_GRAPHICS_PARAMS_H
#define AX_GRAPHICS_PARAMS_H

#include <map>

#include "axle/geom.h"
#include "axle/cg/texture_gl.h"

namespace ax {
template<typename T>
struct __ParamSet {
public:
  typedef T KeyType;

  int GetInt(const KeyType &name) const { 
    return ParamSet::FetchValue(name, this->ints_);    
  }
  uint64 GetUint64(const KeyType &name) const {
    return ParamSet::FetchValue(name, this->uint64s_);
  }
  bool GetBool(const KeyType &name) const { 
    return ParamSet::FetchValue(name, this->bools_);
  }
  float GetFloat(const KeyType &name) const {
    return ParamSet::FetchValue(name, this->floats_);
  }
  const KeyType GetStr(const KeyType &name) const {
    return ParamSet::FetchValue(name, this->strs_);
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
  template<typename T>
  static typename T::mapped_type FetchValue(
      const typename T::key_type &key, const T &c) {    
    typename T::const_iterator it = c.find(key);
    assert(it != c.end());
    if (it != c.end()) return it->second;
    return T::mapped_type();
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
  ax::Matrix4x4 GetMat4(const KeyType &name) const {
    return ParamSet::FetchValue(name, this->mats_);
  }
  ax::Vector3 GetVec3(const KeyType &name) const {
    return ParamSet::FetchValue(name, this->vecs_);
  }
  template<typename PTR>
  PTR GetPtr(const KeyType &name) const {
    return (PTR)ParamSet::FetchValue(name, this->ptrs_);
  }
  ax::Texture2DPtr GetTexture(const KeyType &name) const {
    return ParamSet::FetchValue(name, this->textures_);
  }

  void SetMat4(const KeyType &name, const ax::Matrix4x4 &m) {
    this->mats_[name] = m;
  }
  void SetVec3(const KeyType &name, const ax::Vector3 &v) {
    this->vecs_[name] = v;
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

  Mat4Params mats_;
  Vec3Params vecs_;
  PtrParams ptrs_;
  TexParams textures_;
};

typedef __ParamSet<std::string> ParamSet;
typedef __ParamSet2<std::string> ParamSet2;

}

#endif // AX_GRAPHICS_PARAMS_H

