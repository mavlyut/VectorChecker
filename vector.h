#pragma once
#include <cstddef>

template <typename T>
struct vector {
  using iterator = T*;
  using const_iterator = T const*;

  vector() = default;                                   // O(1) nothrow

  vector(vector const& other) {                         // O(N) strong
    copy(other.data_, other.size_, other.size_);
  }

  vector& operator=(vector const& other) {              // O(N) strong
    if (this != &other) {
      vector(other).swap(*this);
    }
    return *this;
  }

  ~vector() {                                           // O(N) nothrow
    clear();
    operator delete(data_);
  }

  T& operator[](size_t i) {                             // O(1) nothrow
    return data_[i];
  }

  T const& operator[](size_t i) const {                 // O(1) nothrow
    return data_[i];
  }

  T* data() {                                           // O(1) nothrow
    return data_;
  }

  T const* data() const {                               // O(1) nothrow
    return data_;
  }

  size_t size() const {                                 // O(1) nothrow
    return size_;
  }

  T& front() {                                          // O(1) nothrow
    return data_[0];
  }

  T const& front() const {                              // O(1) nothrow
    return data_[0];
  }

  T& back() {                                           // O(1) nothrow
    return data_[size_ - 1];
  }

  T const& back() const {                               // O(1) nothrow
    return data_[size_ - 1];
  }

  void push_back(T const& x) {                          // O(1)* strong
    if (size_ == capacity_) {
      size_t new_capacity = std::max<size_t>(1, 2 * capacity_);
      T* new_data = static_cast<T*>(operator new(new_capacity * sizeof(T)));
      size_t tmp_size = 0;
      try {
        for (; tmp_size < size_; tmp_size++) {
          new (new_data + tmp_size) T(data_[tmp_size]);
          data_[tmp_size].~T();
        }
        new (new_data + size_) T(x);
        for (size_t i = 0; i < size_; i++) {
          data_[i].~T();
        }
        operator delete(data_);
        data_ = new_data;
        capacity_ = new_capacity;
      } catch (...) {
        for (size_t i = 0; i < tmp_size; i++) {
          (new_data + i)->~T();
        }
        operator delete(new_data);
        throw;
      }
    } else {
      new (data_ + size_) T(x);
    }
    size_++;
  }

  void pop_back() {                                     // O(1) nothrow
    data_[--size_].~T();
  }

  bool empty() const {                                  // O(1) nothrow
    return size_ == 0;
  }

  size_t capacity() const {                             // O(1) nothrow
    return capacity_;
  }

  void reserve(size_t new_capacity) {                   // O(N) strong
    if (new_capacity > capacity_) {
      set_capacity(new_capacity);
    }
  }

  void shrink_to_fit() {                                // O(N) strong
    set_capacity(size_);
  }

  void clear() {                                        // O(N) nothrow
    erase(begin(), end());
  }

  void swap(vector& other) {                            // O(1) nothrow
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
  }

  iterator begin() {                                    // O(1) nothrow
    return data_;
  }

  iterator end() {                                      // O(1) nothrow
    return data_ + size_;
  }

  const_iterator begin() const {                        // O(1) nothrow
    return data_;
  }

  const_iterator end() const {                          // O(1) nothrow
    return data_ + size_;
  }

  iterator insert(const_iterator pos, T const& ins) {   // O(N) strong
    ptrdiff_t diff = pos - begin();
    push_back(ins);
    for (size_t i = size_ - 1; i - diff > 0; i--) {
      std::swap(data_[i], data_[i - 1]);
    }
    return begin() + diff;
  }

  iterator erase(const_iterator pos) {                  // O(N) nothrow(swap)
    return erase(pos, pos + 1);
  }

  iterator erase(const_iterator first, const_iterator last) {       // O(N) nothrow(swap)
    ptrdiff_t count = last - first;
    ptrdiff_t start = first - begin();
    for (size_t i = start; i < size_ - count; i++) {
      std::swap(data_[i], data_[i + count]);
    }
    for (size_t i = 0; i < count; ++i) {
      pop_back();
    }
    return begin() + start;
  }

private:
  T* data_{nullptr};
  size_t size_{0};
  size_t capacity_{0};

  void copy(T* data, size_t size, size_t capacity) {                  // O(N)
    if (size <= capacity) {
      T* tmp_data = nullptr;
      if (capacity > 0) {
        tmp_data = static_cast<T*>(operator new(capacity * sizeof(T)));
      }
      size_t tmp_size = 0;
      try {
        for (; tmp_size < size; tmp_size++) {
          new (tmp_data + tmp_size) T(data[tmp_size]);
        }
        clear();
        operator delete(data_);
        size_ = size;
        capacity_ = capacity;
        data_ = tmp_data;
      } catch (...) {
        for (size_t i = 0; i < tmp_size; i++) {
          (tmp_data + i)->~T();
        }
        operator delete(tmp_data);
        throw;
      }
    }
  }

  void set_capacity(size_t new_capacity) {                          // O(N)
    if (new_capacity != capacity_) {
      copy(data_, size_, new_capacity);
    }
  }
};
