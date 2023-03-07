#pragma once
#include <cstddef>

template <typename T>
struct vector {
  using iterator = T*;
  using const_iterator = T const*;

  vector();                               // O(1) nothrow
  vector(vector const&);                  // O(N) strong
  vector& operator=(vector const& other); // O(N) strong

  ~vector(); // O(N) nothrow

  T& operator[](size_t i);             // O(1) nothrow
  T const& operator[](size_t i) const; // O(1) nothrow

  T* data();             // O(1) nothrow
  T const* data() const; // O(1) nothrow
  size_t size() const;   // O(1) nothrow

  T& front();             // O(1) nothrow
  T const& front() const; // O(1) nothrow

  T& back();                // O(1) nothrow
  T const& back() const;    // O(1) nothrow
  void push_back(T const&); // O(1)* strong
  void pop_back();          // O(1) nothrow

  bool empty() const; // O(1) nothrow

  size_t capacity() const; // O(1) nothrow
  void reserve(size_t);    // O(N) strong
  void shrink_to_fit();    // O(N) strong

  void clear(); // O(N) nothrow

  void swap(vector&); // O(1) nothrow

  iterator begin(); // O(1) nothrow
  iterator end();   // O(1) nothrow

  const_iterator begin() const; // O(1) nothrow
  const_iterator end() const;   // O(1) nothrow

  iterator insert(const_iterator pos, T const&); // O(N) strong
  
  iterator erase(const_iterator pos); // O(N) nothrow(swap)

  iterator erase(const_iterator first, const_iterator last); // O(N) nothrow(swap)

private:
  void destroy_elements() {
    for (size_t i = size_; i > 0; i--) {
      data_[i - 1].~T();
    }
  }

  void change_capacity(size_t new_cap) {
    T* new_data = new_cap == 0 ? nullptr : static_cast<T*>
                    (operator new (new_cap * sizeof(T)));
    for (size_t i = 0; i != size_; ++i) {
      try {
        new (new_data + i) T(data_[i]);
      } catch (...) {
        for(size_t j = i; j > 0; --j) {
          new_data[j - 1].~T();
        }
        operator delete(new_data);
        throw;
      }
    }
    destroy_elements();
    operator delete(data_);
    data_ = new_data;
    capacity_ = new_cap;
  }

  void change_cap_push_back(size_t new_cap, T const& value);

  void push_back_without_extend_checking(T const& value){
    new (end()) T(value);
    ++size_;
  }

  T* data_ = nullptr;
  size_t size_{0};
  size_t capacity_{0};
};

template <typename T>
vector<T>::vector() = default;

template <typename T>
vector<T>::vector(vector<T> const &other) : vector() {
  reserve(other.size());
  for (size_t i = 0; i != other.size_; ++i) {
    push_back_without_extend_checking(other.data_[i]);
  }
}

template <typename T>
vector<T>& vector<T>::operator=(vector<T> const& other) {
  if (this != &other) {
    vector(other).swap(*this);
  }
  return *this;
}

template <typename T>
vector<T>::~vector() {
  destroy_elements();
  operator delete(data_);
}

template <typename T>
T& vector<T>::operator[](size_t index) {
  return data_[index];
}

template <typename T>
const T& vector<T>::operator[](size_t index) const {
  return data_[index];
}

template <typename T>
T* vector<T>::data() {
  return data_;
}

template <typename T>
const T* vector<T>::data() const {
  return data_;
}

template <typename T>
size_t vector<T>::size() const {
  return size_;
}

template <typename T>
T& vector<T>::front() {
  return *(begin());
}

template <typename T>
const T& vector<T>::front() const {
  return *(begin());
}

template <typename T>
T& vector<T>::back() {
  return *(end() - 1);
}

template <typename T>
const T& vector<T>::back() const {
  return *(end() - 1);
}

template <typename T>
void vector<T>::push_back(T const& value) {
  if (size() == capacity()) {
    change_cap_push_back(capacity() == 0 ? 1 : capacity() * 2, value);
  } else {
    new (end()) T(value);
    ++size_;
  }
}

template <typename T>
void vector<T>::pop_back() {
  size_--;
  (end())->~T();
}

template <typename T>
bool vector<T>::empty() const {
  return begin() == end();
}

template <typename T>
size_t vector<T>::capacity() const {
  return capacity_;
}

template <typename T>
void vector<T>::reserve(size_t new_capacity) {
  if (capacity() < new_capacity) {
    change_capacity(new_capacity);
  }
}

template <typename T>
void vector<T>::change_cap_push_back(size_t new_cap, T const& value) {
  T* new_data = new_cap == 0 ? nullptr : static_cast<T*>
                    (operator new (new_cap * sizeof(T)));
  for (size_t i = 0; i != size_; ++i) {
    try {
      new (new_data + i) T(data_[i]);
    } catch (...) {
      for(size_t j = i; j > 0; --j) {
        new_data[j - 1].~T();
      }
      operator delete(new_data);
      throw;
    }
  }
  try {
    new (new_data + size_) T(value);
  } catch (...) {
    for(size_t j = size(); j > 0; --j) {
      new_data[j - 1].~T();
    }
    operator delete(new_data);
    throw;
  }
  destroy_elements();
  operator delete(data_);
  data_ = new_data;
  ++size_;
  capacity_ = new_cap;
}

template <typename T>
void vector<T>::shrink_to_fit() {
  if (size() < capacity()) {
    change_capacity(size());
  }
}

template <typename T>
void vector<T>::clear() {
  destroy_elements();
  size_ = 0;
}

template <typename T>
void vector<T>::swap(vector<T>& other) {
  std::swap(data_, other.data_);
  std::swap(size_, other.size_);
  std::swap(capacity_, other.capacity_);
}

template <typename T>
typename vector<T>::iterator vector<T>::begin() {
  return data_;
}

template <typename T>
typename vector<T>::iterator vector<T>::end() {
  return data_ + size_;
}

template <typename T>
typename vector<T>::const_iterator vector<T>::begin() const {
  return data_;
}

template <typename T>
typename vector<T>::const_iterator vector<T>::end() const {
  return data_ + size_;
}

template <typename T>
typename vector<T>::iterator vector<T>::insert(const_iterator pos, const T& value) {
  size_t ind = std::distance(begin(),
                             const_cast<vector::iterator>(pos));
  push_back(value);
  for (size_t i = size_ - 1; i > ind; --i) {
    std::swap(data_[i], data_[i - 1]);
  }
  return const_cast<vector::iterator>(pos);
  //return begin() + ind;
}

template <typename T>
typename vector<T>::iterator vector<T>::erase(const_iterator pos) {
  return vector::erase(pos, pos + 1);
}

template <typename T>
typename vector<T>::iterator vector<T>::erase(const_iterator first
                                              , const_iterator last) {
  size_t ind1 = std::distance(begin(),
                              const_cast<vector::iterator>(first));
  size_t ind2 = std::distance(begin(),
                              const_cast<vector::iterator>(last));
  for (size_t i = 0; i < size() - ind2; ++i) {
    std::swap(data_[ind1 + i], data_[ind2 + i]);
  }
  for (size_t i = 0; i < ind2 - ind1; i++) {
    pop_back();
  }
  return const_cast<vector::iterator>(first);
  //return begin() + ind1;
}
