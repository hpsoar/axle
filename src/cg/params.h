#ifndef AX_CG_PARAMS_H
#define AX_CG_PARAMS_H

#include <map>

#include "../geom.h"
#include "cg_fwd.h"
#include <map>

#define PARAM(KEY, T, T_TAG, DEFAULT, RT)                      \
public:                                                        \
  RT Get##T_TAG(const KEY& key, RT val = DEFAULT) const        \
  {                                                            \
    auto it = this->T_TAG##s_.find(key);                       \
    if (it != this->T_TAG##s_.end()) return it->second;        \
    return val;                                                \
  }                                                            \
  void Set##T_TAG(const KEY& key, RT value)                    \
  {                                                            \
    this->T_TAG##s_[key] = value;                              \
  }                                                            \
  void Set##T_TAG##IfNotExist(const KEY &key, RT value)        \
  {                                                            \
    if (!this->Has##T_TAG(key)) this->Set##T_TAG(key, value);  \
  }                                                            \
  bool Has##T_TAG(const KEY& key) const                        \
  {                                                            \
    return this->T_TAG##s_.find(key) != this->T_TAG##s_.end(); \
  }                                                            \
private:                                                       \
  typedef std::map<KEY, T> T_TAG##Params;                      \
  T_TAG##Params T_TAG##s_;                                     \

#define POD_PARAM(KEY, T, T_TAG, DEFAULT) PARAM(KEY, T, T_TAG, DEFAULT, T)
#define NPOD_PARAM(KEY, T, T_TAG, DEFAULT) PARAM(KEY, T, T_TAG, DEFAULT, const T&)

namespace ax {
template<typename T>
struct __ParamSet {
public:
  typedef T KeyType;

  /* example:
   * int GetInt(const KeyType &key, int val = 0) const;
   * void SetInt(const KeyType &key, int val);
   * bool HasInt(const KeyType &key) const;
   */
  POD_PARAM(KeyType, int, Int, 0)
  POD_PARAM(KeyType, float, Float, 0.f)
  POD_PARAM(KeyType, bool, Bool, false)
  POD_PARAM(KeyType, uint64, Uint64, 0)

  /* example:
   * const std::string &GetStr(const KeyType &key, const std::string &val = "") const;
   * void SetStr(const KeyType &key, const std::string &val);
   * bool HasStr(const KeyType &key);
   */
  NPOD_PARAM(KeyType, std::string, Str, "")
};

template<typename T>
struct __ParamSet2 : public __ParamSet<T> {
public:  
  typedef typename __ParamSet<T>::KeyType KeyType;
  NPOD_PARAM(KeyType, ax::Matrix4x4, Mat4, ax::Matrix4x4(1.f))
  NPOD_PARAM(KeyType, ax::Vector3, Vec3, ax::Vector3(0))
  NPOD_PARAM(KeyType, ax::Texture2DPtr, Tex2D, NULL)
  POD_PARAM(KeyType, const void*, RawPtr, NULL)
   
public:
  template<typename PTR>
  PTR GetPtr(const KeyType &key) const { return (PTR)this->GetRawPtr(key); }

  template<typename PTR>
  void SetPtr(const KeyType &key, PTR ptr) { this->SetRawPtr(key, (const void*)ptr); }
};

typedef std::string CGParamKeyType;

typedef __ParamSet<CGParamKeyType> ParamSet;
typedef __ParamSet2<CGParamKeyType> ParamSet2;

#define DEF_PARAM_NAME(NS, NAME)          \
  namespace NS {                          \
    const std::string NAME = #NAME;       \
  }                                       \

}

#endif // AX_CG_PARAMS_H

