#ifndef AXLE_CORE_CONTAINER_H
#define AXLE_CORE_CONTAINER_H

#include <list>

namespace ax {
template<typename T>
struct Node {
  Node() : next(NULL) { }
  T n;
  Node *next;
};

template<typename T, int kBlockSize>
class AllocOnlyAllocator {
public:
  AllocOnlyAllocator() : 
      block_(NULL), blockused_(kBlockSize), registered_lists_(0) { }
  T *Allocate() {    
    if (blockused_ == kBlockSize) this->AllocateBlock();

    T *ptr = block_ + blockused_;
    ++blockused_;
    return ptr;
  }
  
  ~AllocOnlyAllocator() { this->Release(); }

  void Register() { ++registered_lists_; }
  void Unregister() { 
    --registered_lists_;
    if (registered_lists_ == 0) this->Release();
  }
private:
  void AllocateBlock() {
    block_ = new T[kBlockSize];
    blocks_.push_back(block_);
    blockused_ = 0;
  }
  void Release() {
    typename std::list<T*>::iterator it = blocks_.begin();
    for ( ; it != blocks_.end(); ++it) delete (*it);
    blockused_ = kBlockSize;
    blocks_.clear();
  }
private:
  std::list<T*> blocks_;
  T *block_;
  int blockused_;
  int registered_lists_;
};

template<typename L>
struct Iterator {
public:
  typedef typename L::node_type node_type;
  typedef typename L::value_type value_type;

  Iterator(node_type *ptr) : ptr(ptr) { }      

  value_type operator*() { return ptr->n; }
  Iterator &operator++() {
    ptr = ptr->next;
    return *this;
  }
private:
  node_type *ptr;

  template<typename LL>
  friend class ConstIterator;
};

template<typename L>
class ConstIterator {
public:
  typedef typename L::node_type node_type;
  typedef typename L::value_type value_type;

  ConstIterator(node_type *ptr) : ptr(ptr) { }

  ConstIterator(const Iterator<L> &it) : ptr(it.ptr) { }

  value_type operator*() { return ptr->n; }
  ConstIterator &operator++() { 
    ptr = ptr->next;
    return *this;
  }
  template<typename IT>
  bool operator==(const IT &it) const { return ptr == it.ptr; }
  template<typename IT>
  bool operator!=(const IT &it) const { return ptr != it.ptr; }
private:
  const node_type *ptr;  
};

template<typename T, int kBlockSize = 512>
class List {
public:
  typedef T value_type;
  typedef Node<T> node_type;

  typedef Iterator<List> iterator;
  typedef ConstIterator<List> const_iterator;

  List() : head_(NULL), tail_(NULL), size_(0) {
    allocator_.Register();
    head_ = allocator_.Allocate();
    tail_ = head_;
  }

  ~List() { 
    allocator_.Unregister(); 
  }

  void push_back(const T &t) {
    node_type *n = allocator_.Allocate();
    n->n = t;
    tail_->next = n;
    tail_ = n;
    ++size_;
  }

  void push_front(const T &t) {
    node_type *n = allocator_.Allocate();
    n->n = t;
    n->next = head_->next;
    head_->next = n;
    ++size_;
  }

  void insert(const T &t) {
    node_type *p = head_; 
    node_type *it = p->next;
    for (; it != NULL; p = it, it = it->next) {
      if (it->n == t) return;
      if (it->n > t) break;
    }
    node_type *n = allocator_.Allocate();
    n->n = t;
    p->next = n;
    n->next = it;
    if (tail_ == p) tail_ = n;
    ++size_;
  }
  
  template<typename Iter>
  void assign(Iter begin, Iter end) {
    for (; begin != end; ++begin) {
      this->push_back(*begin);
    }
  }

  size_t size() const { return size_; }

  iterator begin() { return iterator(head_->next); }
  const_iterator begin() const { return const_iterator(head_->next); }
  const_iterator end() const { return const_iterator(NULL); }
private:
  node_type *head_, *tail_;
  size_t size_;
  static AllocOnlyAllocator<Node<T>, kBlockSize> allocator_;
};

template<typename T, int kBlockSize>
AllocOnlyAllocator<Node<T>, kBlockSize> List<T, kBlockSize>::allocator_;

template<typename T>
class Array {
public:
  Array(size_t n) { data_ = new T[n]; n_ = n; }
  ~Array() { delete[] data_; data_ = NULL; }

  T &operator[](size_t i) { return data_[i]; }
  const T &operator[](size_t i) const { return data_[i]; }

  size_t size() const { return n_; }
private:
  T *data_;
  size_t n_;
};
} // ax

#endif // AXLE_CORE_CONTAINER_H
