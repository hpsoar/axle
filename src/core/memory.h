#ifndef AXLE_CORE_MEMORY_H
#define AXLE_CORE_MEMORY_H

#include <vector>

#include "../core/settings.h"
#include "../core/typedefs.h"
#include "../core/macros.h"
#include "../core/debug.h"

#include <string.h>

namespace ax {
template<size_t alignment >
class AlignedAllocator {
public:
  const static size_t s_alignment = alignment;
  //const static unsigned int s_alignment_mask = (1<<(unsigned int)alignment)-1;

  template<typename T>
  static T *Allocate(size_t count) {
    return (T*)Allocate(count * sizeof(T));
  }
  static void *Allocate(size_t size) {
#if defined(SYS_IS_WINDOWS)
    return _aligned_malloc(size, s_alignment);
#elif defined(SYS_IS_APPLE) || defined(SYS_IS_OPENBSD)
    Logger::Log("untested function called at: %d", __LINE__);
    void *mem = malloc(size + (AXLE_L1_CACHE_LINE_SIZE-1) + sizeof(void*));
    char *amem = ((char*)mem) + sizeof(void*);
#if (AXLE_POINTER_SIZE==8)
    amem += AXLE_L1_CACHE_LINE_SIZE - (reinterpret_cast<uint64_t>(amem) & (AXLE_L1_CACHE_LINE_SIZE - 1));
#else
    amem += AXLE_L1_CACHE_LINE_SIZE - (reinterpret_cast<uint32_t>(amem) & (AXLE_L1_CACHE_LINE_SIZE - 1));
#endif
      ((void**)amem)[-1] = mem;
      return amem;
#else // Linux
    return memalign(s_alignment, size);
#endif
  }
  static void Deallocate(void *ptr) {
    if (ptr == NULL) return;
#if defined(SYS_IS_WINDOWS)
    return _aligned_free(ptr);
#elif defined(SYS_IS_LINUX)
    return free(ptr);
#endif
  }
private:
  DISABLE_INSTANCE(AlignedAllocator);
};

typedef AlignedAllocator<AXLE_L1_CACHE_LINE_SIZE> DefaultAlignedAllocator;

class MemoryArena {
public:
  MemoryArena(uint32_t bs = 32768) {
    block_size_ = bs;
    current_block_pos_ = 0;
    current_block_ = DefaultAlignedAllocator::Allocate<char>(block_size_);
  }
  ~MemoryArena() {
    DefaultAlignedAllocator::Deallocate(current_block_);
    for (uint32_t i = 0; i < used_blocks_.size(); ++i)
      DefaultAlignedAllocator::Deallocate(used_blocks_[i]);
    for (uint32_t i = 0; i < available_blocks_.size(); ++i)
      DefaultAlignedAllocator::Deallocate(available_blocks_[i]);
  }
  void *Alloc(uint32_t sz) {
    // Round up _sz_ to minimum machine alignment
    sz = ((sz + 15) & (~15));
    if (current_block_pos_ + sz > block_size_) {
      // Get new block of memory for _MemoryArena_
      used_blocks_.push_back(current_block_);
      if (available_blocks_.size() && sz <= block_size_) {
        current_block_ = available_blocks_.back();
        available_blocks_.pop_back();
      } else {
        current_block_ = DefaultAlignedAllocator::Allocate<char>(
            std::max(sz, block_size_));
      }
      current_block_pos_ = 0;
    }
    void *ret = current_block_ + current_block_pos_;
    current_block_pos_ += sz;
    return ret;
  }
  template<typename T> T *Alloc(uint32_t count = 1) {
    T *ret = (T *)Alloc(count * sizeof(T));
    for (uint32_t i = 0; i < count; ++i)
      new (&ret[i]) T();
    return ret;
  }
  void FreeAll() {
    current_block_pos_ = 0;
    while (used_blocks_.size()) {
  #ifndef NDEBUG
      memset(used_blocks_.back(), 0xfa, block_size_);
  #endif
      available_blocks_.push_back(used_blocks_.back());
      used_blocks_.pop_back();
    }
  }
private:
  // MemoryArena Private Data
  uint32_t current_block_pos_, block_size_;
  char *current_block_;
  std::vector<char*> used_blocks_, available_blocks_;
};
} // ax
#endif // AXLE_CORE_MEMORY_H
