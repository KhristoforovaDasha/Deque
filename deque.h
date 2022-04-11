#pragma once
#include <iostream>
#include <iterator>
template<typename Type>
class Deque {
 public:
  Deque() {
    capacity_ = 1;
    allocate();
  }
  Deque(const Deque<Type>& other);
  Deque(size_t count);
  Deque(size_t count, const Type& value);
  Deque<Type>& operator=(const Deque& other);
  void swap(Deque& other);
  size_t size() const {
    return size_;
  }
  Type& operator[](size_t index);
  const Type& operator[](size_t index) const;
  Type& at(size_t index);
  const Type& at(size_t index) const;
  void push_back(const Type& elem);
  void push_front(const Type& elem);
  void pop_back();
  void pop_front();
  ~Deque();
  template<bool isConst>
  class common_iterator {
   public:
    friend class Deque;
    using difference_type = std::ptrdiff_t;;
    using iterator_category = std::bidirectional_iterator_tag;
    using pointer = std::conditional_t<isConst, const Type*, Type*>;
    using reference = std::conditional_t<isConst, const Type&, Type&>;
    using value_type = std::conditional_t<isConst, const Type, Type>;
    common_iterator(Type*& it, size_t position)
        : it_(&it), position_(position) {}
    common_iterator(const common_iterator& copy)
        : it_(copy.it_), position_(copy.position_) {}
    common_iterator& operator=(const common_iterator& other) {
      common_iterator copy(other);
      swap(copy);
      return *this;
    }
    void swap(common_iterator& copy) {
      std::swap(it_, copy.it_);
      std::swap(position_, copy.position_);
    }
    common_iterator& operator++() {
      if (position_ + 1 == END) ++it_;
      position_ = (position_ + 1) % block;
      return *this;
    }
    common_iterator operator++(int) {
      common_iterator copy = *this;
      ++(*this);
      return copy;
    }
    common_iterator& operator--() {
      if (position_ == BEGIN) --it_;
      position_ = (position_ + block - 1) % block;
      return *this;
    }
    common_iterator operator--(int) {
      common_iterator copy = *this;
      --(*this);
      return copy;
    }
    bool operator==(const common_iterator<isConst>& other) const {
      return (*this <= other) && (*this >= other);
    }
    bool operator<(const common_iterator<isConst>& other) const {
      if (*it_ == *(other.it_)) return position_ < other.position_;
      return *it_ < *(other.it_);
    }
    bool operator>(const common_iterator<isConst>& other) const {
      return other < *this;
    }
    bool operator<=(const common_iterator<isConst>& other) const {
      return !(*this > other);
    }
    bool operator>=(const common_iterator<isConst>& other) const {
      return !(*this < other);
    }
    bool operator!=(const common_iterator<isConst>& other) const {
      return !(*this == other);
    }
    reference operator*() const {
      return *((*it_) + position_);
    }
    pointer operator->() const {
      return &((*it_)[position_]);
    }
    common_iterator operator+(const size_t count) {
      common_iterator result = *this;
      if (result.position_ + count >= block) {
        result.it_ += (count + result.position_) / block;
      }
      result.position_ = (result.position_ + count) % block;
      return result;
    }
    common_iterator operator-(const size_t count) {
      common_iterator result = *this;
      if (result.position_ < count) {
        result.it_ -= (count - result.position_ + block - 1) / block;
      }
      result.position_ = (result.position_ + block - count % block) % block;
      return result;
    }
    int operator-(const common_iterator<isConst>& other) const {
      int result = (it_ - other.it_) * block + static_cast<int>(position_)
          - static_cast<int>(other.position_);
      return result;
    }
    explicit operator common_iterator<true>() {
      return common_iterator<true>(it_, position_);
    }
   private:
    Type** it_;
    size_t position_ = 0;
  };
  using iterator = common_iterator<false>;
  using const_iterator = common_iterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  iterator begin();
  const_iterator begin() const;
  const_iterator cbegin() const;
  iterator end();
  const_iterator end() const;
  const_iterator cend() const;
  reverse_iterator rbegin();
  const_reverse_iterator rbegin() const;
  const_reverse_iterator crbegin() const;
  reverse_iterator rend();
  const_reverse_iterator rend() const;
  const_reverse_iterator crend() const;
  void insert(iterator it, const Type& elem);
  void erase(iterator it);
 private:
  Type** deque;
  size_t first_ = 0, first_block_ = 0;
  size_t last_ = 0, last_block_ = 0;
  size_t capacity_ = 0, size_ = 0;
  static const int block = 8;

  void reallocate();
  void allocate();
  void delete_deque();
  enum Place {
    BEGIN = 0,
    END = block
  };
};
template<typename Type>
Deque<Type>::Deque(const Deque<Type>& other) :
    first_(other.first_),
    first_block_(other.first_block_),
    last_(other.last_),
    last_block_(other.last_block_),
    capacity_(other.capacity_),
    size_(other.size_) {
  allocate();
  size_t i;
  try {
    for (i = first_block_; i <= last_block_; ++i) {
      size_t begin = 0, end = block;
      if (i == first_block_) begin = first_;
      if (i == last_block_) end = last_;
      for (size_t j = begin; j < end; ++j) {
        new(deque[i] + j) Type(other.deque[i][j]);
      }
    }
  } catch (...) {
    for (size_t ind = 0; ind <= i; ++ind) {
      (*this)[ind].~Type();
    }
    delete_deque();
    throw;
  }
}
template<typename Type>
Deque<Type>::Deque(size_t count) {
  size_ = count;
  capacity_ = 3 * (count / block + 1);
  first_block_ = capacity_ / 2;
  last_block_ = first_block_ + count / block;
  first_ = 0, last_ = count % block;
  allocate();
}
template<typename Type>
Deque<Type>::Deque(size_t count, const Type& value) :Deque(count) {
  size_t ind;
  try {
    for (ind = first_block_; ind <= last_block_; ++ind) {
      size_t begin = 0, end = block;
      if (ind == first_block_) begin = first_;
      if (ind == last_block_) end = last_;
      for (size_t j = begin; j < end; ++j) {
        new(deque[ind] + j) Type(value);
      }
    }
  }
  catch (...) {
    for (size_t i = 0; i <= ind; ++i) {
      (*this)[i].~Type();
    }
    delete_deque();
    throw;
  }
}
template<typename Type>
Deque<Type>& Deque<Type>::operator=(const Deque& other) {
  Deque<Type> copy(other);
  swap(copy);
  return *this;
}
template<typename Type>
void Deque<Type>::swap(Deque& other) {
  if (this != &other) {
    std::swap(size_, other.size_);
    std::swap(first_, other.first_);
    std::swap(first_block_, other.first_block_);
    std::swap(last_, other.last_);
    std::swap(last_block_, other.last_block_);
    std::swap(capacity_, other.capacity_);
    std::swap(deque, other.deque);
  }
}
template<typename Type>
Type& Deque<Type>::at(size_t index) {
  if (index >= size_) throw std::out_of_range("out of range");
  return (*this)[index];
}
template<typename Type>
const Type& Deque<Type>::at(size_t index) const {
  if (index >= size_) throw std::out_of_range("out of range");
  return (*this)[index];
}
template<typename Type>
void Deque<Type>::push_back(const Type& elem) {
  size_t prev_last = last_;
  size_t prev_last_block = last_block_;
  try {
    new(deque[last_block_] + last_) Type(elem);
    ++size_;
  } catch (...) {
    deque[last_block_][last_].~Type();
    last_ = prev_last;
    last_block_ = prev_last_block;
    throw;
  }
  if (last_block_ + 1 >= capacity_ && last_ + 1 >= END) {
    reallocate();
  }
  if (last_ + 1 == END) {
    ++last_block_;
    last_ = BEGIN;
  } else {
    ++last_;
  }
}
template<typename Type>
void Deque<Type>::push_front(const Type& elem) {
  size_t prev_first = first_;
  size_t prev_first_block = first_block_;
  if (first_block_ == 0 && first_ == BEGIN) {
    reallocate();
  }
  if (first_ == BEGIN) {
    --first_block_;
    first_ = END - 1;
  } else {
    --first_;
  }
  try {
    new(deque[first_block_] + first_) Type(elem);
    ++size_;
  } catch (...) {
    deque[first_block_][first_].~Type();
    first_ = prev_first;
    first_block_ = prev_first_block;
  }
}
template<typename Type>
void Deque<Type>::pop_back() {
  if (last_ == 0) {
    --last_block_;
  }
  last_ = (last_ + block - 1) % block;
  deque[last_block_][last_].~Type();
  --size_;
}
template<typename Type>
void Deque<Type>::pop_front() {
  deque[first_block_][first_].~Type();
  if (first_ + 1 == END) {
    ++first_block_;
  }
  first_ = (first_ + 1) % block;
  --size_;
}
template<typename Type>
void Deque<Type>::reallocate() {
  Type** current = new Type* [3 * capacity_];
  for (size_t i = 0; i < 3 * capacity_; ++i) {
    if (i >= capacity_ && i < 2 * capacity_) {
      current[i] = deque[i - capacity_];
    } else {
      current[i] = reinterpret_cast<Type*>(new uint8_t[block * sizeof(Type)]);
    }
  }
  delete[] deque;
  deque = current;
  first_block_ += capacity_;
  last_block_ += capacity_;
  capacity_ *= 3;
}
template<typename Type>
void Deque<Type>::allocate() {
  deque = new Type* [capacity_];
  for (size_t i = 0; i < capacity_; ++i) {
    deque[i] = reinterpret_cast<Type*>(new uint8_t[block * sizeof(Type)]);
  }
}
template<typename Type>
void Deque<Type>::delete_deque() {
  for (size_t i = 0; i < capacity_; ++i) {
    delete[] reinterpret_cast<uint8_t*>(deque[i]);
  }
  delete[] deque;
}
template<typename Type>
Deque<Type>::~Deque() {
  for (size_t i = 0; i < size_; ++i) {
    (*this)[i].~Type();
  }
  delete_deque();
}
template<typename Type>
Type& Deque<Type>::operator[](size_t index) {
  if (index < (block - first_)) {
    return deque[first_block_][first_ + index];
  }
  return deque[first_block_ + (index - (block - first_)) / block + 1][
      (index - (block - first_)) % block];
}
template<typename Type>
const Type& Deque<Type>::operator[](size_t index) const {
  if (index < (block - first_)) return deque[first_block_][first_ + index];
  return deque[first_block_ + (index - (block - first_)) / block + 1][
      (index - (block - first_)) % block];
}
template<typename Type>
typename Deque<Type>::iterator Deque<Type>::begin() {
  return iterator(deque[first_block_], first_);
}
template<typename Type>
typename Deque<Type>::const_iterator Deque<Type>::begin() const {
  return const_iterator(deque[first_block_], first_);
};
template<typename Type>
typename Deque<Type>::const_iterator Deque<Type>::cbegin() const {
  return const_iterator(deque[first_block_], first_);
};
template<typename Type>
typename Deque<Type>::iterator Deque<Type>::end() {
  return iterator(deque[last_block_], last_);
}
template<typename Type>
typename Deque<Type>::const_iterator Deque<Type>::end() const {
  return const_iterator(deque[last_block_], last_);
};
template<typename Type>
typename Deque<Type>::const_iterator Deque<Type>::cend() const {
  return const_iterator(deque[last_block_], last_);
}
template<typename Type>
typename Deque<Type>::reverse_iterator Deque<Type>::rbegin() {
  return reverse_iterator(end());
}
template<typename Type>
typename Deque<Type>::const_reverse_iterator Deque<Type>::rbegin() const {
  return const_reverse_iterator(end());
};
template<typename Type>
typename Deque<Type>::const_reverse_iterator Deque<Type>::crbegin() const {
  return const_reverse_iterator(end());
};

template<typename Type>
typename Deque<Type>::reverse_iterator Deque<Type>::rend() {
  return reverse_iterator(begin());
}
template<typename Type>
typename Deque<Type>::const_reverse_iterator Deque<Type>::rend() const {
  return const_reverse_iterator(begin());
};
template<typename Type>
typename Deque<Type>::const_reverse_iterator Deque<Type>::crend() const {
  return const_reverse_iterator(begin());
};

template<typename Type>
void Deque<Type>::insert(Deque::iterator index, const Type& elem) {
  if (size_ == 0) push_back(elem);
  else {
    iterator current = (--end());
    Type last = *current;
    while (current != index) {
      (*(current.it_))[current.position_] =
          (*((current - 1).it_))[(current - 1).position_];
      --current;
    }
    (*(index.it_))[index.position_] = elem;
    push_back(last);
  }
}
template<typename Type>
void Deque<Type>::erase(Deque::iterator index) {
  iterator last = end();
  while (last != index) {
    (*(index.it_))[index.position_] =
        (*((index + 1).it_))[(index + 1).position_];
    index++;
  }
  pop_back();
}

