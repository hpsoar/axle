#ifndef AXLE_IMG_IMAGE_IL_H
#define AXLE_IMG_IMAGE_IL_H

#include <string.h>
#include <IL/il.h>
#include <IL/ilu.h>

#include "pixel.hpp"

#include "../core/memory.h"

namespace ax {

/*
 * @return : true if error occurred
 */
inline void CheckErrorsIL() {
  ILenum error;  
  while ((error = ilGetError()) != IL_NO_ERROR) {
    printf("%d: %s/n", error, iluErrorString(error));    
  }  
}

template<ILint FORMAT, ILint TYPE, ILint N>
struct DevILFormat__ {
  const static ILint format = FORMAT;
  const static ILint type = TYPE;
  const static ILint depth = 1;
  const static ILint n_channels = N;
};

template<typename PIXEL>
struct DevILFormat { };

template<> struct DevILFormat<PixelByte> 
    : DevILFormat__<IL_LUMINANCE, IL_UNSIGNED_BYTE, 1> { };

template<> struct DevILFormat<PixelFloat> 
    : DevILFormat__<IL_LUMINANCE, IL_FLOAT, 1> { };

template<> struct DevILFormat<PixelRgbByte> 
    : DevILFormat__<IL_BGR, IL_UNSIGNED_BYTE, 3> { };

template<> struct DevILFormat<PixelRgbaByte> 
    : DevILFormat__<IL_BGRA, IL_UNSIGNED_BYTE, 4> { };

template<> struct DevILFormat<PixelRgbFloat> 
    : DevILFormat__<IL_BGR, IL_FLOAT, 3> { };

template<> struct DevILFormat<PixelRgbaFloat> 
    : DevILFormat__<IL_BGRA, IL_FLOAT, 4> { };

template<typename PIXEL>
class ImageIL {
public:
  typedef PIXEL PixelType;

  ImageIL() : data_(NULL), width_(0), height_(0), image_id_(0) {
    this->Initialize();
  }

  ~ImageIL() { this->Release(); }

  int Allocate(int width, int height) {
    assert(width > 0 && height > 0);
    if (width <= 0 || height <= 0) return kInvalidArg;

    if (width == this->width() && height == this->height())
      return kOK;

    // if AllocateUnchecked failed, nothing will change
    ILuint id = AllocateUnchecked(width, height);

    int ret = kOK;
    if (0 == id) {
      ret = kFailed;
    } else {
      // if Release failed, maintain old state,
      // unless ilDeleteImage modified image data
      this->Release();
    
      // should not fail
      this->UpdateCache();
    }

    ilBindImage(0);
    CheckErrorsIL();
    return kOK;
  }

  /*
   * don't allow to Load to an allocated image
   */
  int Load(const std::string &filename) {
    ILuint temp_img = ilGenImage();
    ilBindImage(temp_img);
    
    if (!ilLoadImage(filename.c_str())) {
      // error
      ilDeleteImage(temp_img);
      ilBindImage(0);

      CheckErrorsIL();
      return kFailed;
    }
    
    // the following gets shouldn't fail, if image loading succeed
    ILint width = ilGetInteger(IL_IMAGE_WIDTH);
    ILint height = ilGetInteger(IL_IMAGE_HEIGHT);

    ILint format = ilGetInteger(IL_IMAGE_FORMAT);
    ILint type = ilGetInteger(IL_IMAGE_TYPE);
    ILint depth = ilGetInteger(IL_IMAGE_DEPTH);
    ILint n_channels = ilGetInteger(IL_IMAGE_CHANNELS);

    bool need_convertion = format != s_format || type != s_type ||
                           depth != s_depth || n_channels != s_n_channels;

    if (need_convertion) {
      ILuint id = this->Convert(temp_img, width, height, depth);
      ilDeleteImage(temp_img);
      if (0 == id) return kFailed;

      ilBindImage(id);
    } // else, temp_img is bind

    this->Release();
    
    this->UpdateCache();
    
    ilBindImage(0);
    CheckErrorsIL();

    return kOK;
  }  

  int Save(const std::string &filename) const {
    if (0 == image_id_) return kFailed;
    
    ilEnable(IL_FILE_OVERWRITE);

    int ret = kOK;

    ilBindImage(image_id_);
    if (!ilSaveImage(filename.c_str())) {
      ret = kFailed;
    }

    CheckErrorsIL();
    return ret;
  }

  int Resize(int width, int height) {
    if (0 == image_id_) return kFailed;

    int ret = kOK;

    ilBindImage(image_id_);    
    if (!iluScale(width, height, s_depth)) {
      // it seems that iluScale didn't affect current image,
      // if it fails
      CheckErrorsIL();
      ret = kFailed;
    } else {
      this->UpdateCache();
    }

    return ret;
  }

  int Copy(const ImageIL &image) {
    if (image.IsEmpty()) return kInvalidArg;
    
    int ret = kOK;
    if (image.width() != this->width() || image.height() != this->height()) {
      ILuint id = this->AllocateUnchecked(image.width(), image.height());
      if (0 == id) return kFailed;

      ilBindImage(id);
      if (!ilCopyImage(image.image_id_)) {
        ilDeleteImage(id);
        ret = kFailed;
      } else {
        this->Release();
        this->UpdateCache();
      }
    } else {
      ilBindImage(image_id_);
      if (!ilCopyImage(image.image_id_)) ret = kFailed;
    }
    
    ilBindImage(0);
    CheckErrorsIL();
    return ret;
  }

  template<typename OTHER_PIXEL>
  int Copy(const ImageIL<OTHER_PIXEL> &image) {
    if (image.IsEmpty()) return kInvalidArg;
    
    ILuint id = this->Convert(
        image.image_id_, image.width(), image.height(), image.s_depth);

    if (0 == id) return kFailed;
    
    ilBindImage(id);
    this->Release();
    this->UpdateCache();

    ilBindImage(0);
    CheckErrorsIL();
    return kOK;
  }  
  
  bool IsEmpty() const { return 0 == image_id_;  }

  int width() const { return width_; }
  int height() const { return height_; }

  PIXEL *operator[](int idx) { return data_ + width_ * idx; }
  const PIXEL *operator[](int idx) const { return data_ + width_ * idx; }

private:
  // allocate, and bind
  ILuint AllocateUnchecked(int width, int height) {
    ILuint id = ilGenImage();
    ilBindImage(id);

    bool ret = ilTexImage(width, height, s_depth, s_n_channels,
                          s_format, s_type, NULL);
    if (!ret) {
      ilDeleteImage(id);
      id = 0;
    }

    CheckErrorsIL();
    return id;
  }

  ILuint Convert(ILuint origin, int width, int height, int depth) {
    ILuint id = this->AllocateUnchecked(width, height);
    if (id > 0) {
      // feed the image with loaded data
      ILubyte *data = ilGetData();

      ilBindImage(origin);
      ILuint size = ilCopyPixels(0, 0, 0, width, height, depth,
                                 s_format, s_type, data);
      if (0 == size) {
        ilDeleteImage(id);
        id = 0;
      }
    }
    CheckErrorsIL();
    return id;
  }
  
  // cache image information for currently bind image
  void UpdateCache() {
    image_id_ = ilGetInteger(IL_CUR_IMAGE); 
    data_ = reinterpret_cast<PixelType*>(ilGetData());
    width_ = ilGetInteger(IL_IMAGE_WIDTH);
    height_ = ilGetInteger(IL_IMAGE_HEIGHT);
  }

  void Release() {
    if (image_id_ > 0) {
      ilDeleteImage(image_id_);
      
      image_id_ = 0;
      width_ = 0; height_ = 0;
      data_ = NULL;
    }
  } 

  static void Initialize() {
    static bool initialized = false;
    if (initialized) return;

    ilInit();
    ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
    ilEnable(IL_ORIGIN_SET);

    //iluImageParameter(ILU_FILTER, ILU_LINEAR);

    initialized = true;
  }

private:
  static const ILint s_format = DevILFormat<PixelType>::format;
  static const ILint s_type = DevILFormat<PixelType>::type;
  static const ILint s_depth = DevILFormat<PixelType>::depth;
  static const ILint s_n_channels = DevILFormat<PixelType>::n_channels;

private:
  // image data, and it's convenient for manipulation
  ILuint image_id_;

  // just for convenient access
  int width_, height_;
  PixelType *data_;

  DISABLE_COPY_AND_ASSIGN(ImageIL);

  template<typename OTHER_PIXEL> friend class ImageIL;
};

typedef ImageIL<PixelByte> ImageByte;
typedef ImageIL<PixelFloat> ImageFloat;
typedef ImageIL<PixelRgbByte> ImageRgbByte;
typedef ImageIL<PixelRgbaByte> ImageRgbaByte;
typedef ImageIL<PixelRgbFloat> ImageRgbFloat;
typedef ImageIL<PixelRgbaFloat> ImageRgbaFloat;

} // ax

#endif // AXLE_IMG_IMAGE_IL_H
