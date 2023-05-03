//
// Created by khristoforova-dasha on 21.04.2022.
//

#ifndef DEQUE_DEQUE_H
#define DEQUE_DEQUE_H
#pragma once

#include <iostream>
#include <iterator>

template<typename Type>
class Deque {
public:
  using value_type = Type;

  Deque() {
    capacity_ = 1;
    allocate();
  }

  Deque(const Deque<Type>& other);

  Deque(size_t count, const Type& value = Type());

  Deque<Type>& operator=(const Deque& other);

  void swap(Deque& other) noexcept;

  size_t size() const noexcept {
    return size_;
  }

  Type& operator[](size_t index) noexcept;

  const Type& operator[](size_t index) const noexcept;

  Type& at(size_t index);

  const Type& at(size_t index) const;

  void push_back(const Type& elem);

  void push_front(const Type& elem);

  void pop_back() noexcept;

  void pop_front() noexcept;

  ~Deque() noexcept;

  template<bool isConst>
  class common_iterator {
  public:
    friend class Deque;

    using difference_type = int;
    using iterator_category = std::random_access_iterator_tag;
    using pointer = std::conditional_t<isConst, const Type *, Type *>;
    using reference = std::conditional_t<isConst, const Type&, Type&>;
    using value_type = std::conditional_t<isConst, const Type, Type>;

    common_iterator() {};

    common_iterator(Type *& it, size_t position) noexcept
      : it_(&it), position_(position) {}

    common_iterator(const common_iterator& copy) noexcept
      : it_(copy.it_), position_(copy.position_) {}

    common_iterator& operator=(const common_iterator& other) noexcept {
      common_iterator copy(other);
      swap(copy);
      return *this;
    }

    void swap(common_iterator& copy) noexcept {
      std::swap(it_, copy.it_);
      std::swap(position_, copy.position_);
    }

    common_iterator& operator++() noexcept {
      if (position_ + 1 == END) ++it_;
      position_ = (position_ + 1) % block;
      return *this;
    }

    common_iterator operator++(int) noexcept {
      common_iterator copy = *this;
      ++(*this);
      return copy;
    }

    common_iterator& operator--() noexcept {
      if (position_ == BEGIN) --it_;
      position_ = (position_ + block - 1) % block;
      return *this;
    }

    common_iterator operator--(int) noexcept {
      common_iterator copy = *this;
      --(*this);
      return copy;
    }

    bool operator==(const common_iterator<isConst>& other) const noexcept {
      return (*this <= other) && (*this >= other);
    }

    bool operator<(const common_iterator<isConst>& other) const noexcept {
      if (*it_ == *(other.it_)) return position_ < other.position_;
      return *it_ < *(other.it_);
    }

    bool operator>(const common_iterator<isConst>& other) const noexcept {
      return other < *this;
    }

    bool operator<=(const common_iterator<isConst>& other) const noexcept {
      return !(*this > other);
    }

    bool operator>=(const common_iterator<isConst>& other) const noexcept {
      return !(*this < other);
    }

    bool operator!=(const common_iterator<isConst>& other) const noexcept {
      return !(*this == other);
    }

    reference operator*() const noexcept {
      return *((*it_) + position_);
    }

    pointer operator->() const noexcept {
      return &((*it_)[position_]);
    }

    common_iterator operator+(const difference_type count) const noexcept {
      common_iterator result = *this;
      result += count;
      return result;
    }

    common_iterator operator-(const difference_type count) const noexcept {
      common_iterator result = *this;
      result -= count;
      return result;
    }

    common_iterator& operator+=(const difference_type count) noexcept {
      if (position_ + count >= block) {
        it_ += (count + position_) / block;
      }
      position_ = (position_ + count) % block;
      return *this;
    }

    common_iterator& operator-=(const difference_type count) noexcept {
      if (position_ < count) {
        it_ -= (count - position_ + block - 1) / block;
      }
      position_ = (position_ + block - count % block) % block;
      return *this;
    }

    difference_type operator-(const common_iterator<isConst>& other) const noexcept {
      int result = (it_ - other.it_) * block + static_cast<int>(position_)
                   - static_cast<int>(other.position_);
      return result;
    }

    operator common_iterator<true>() noexcept {
      return common_iterator<true>(it_, position_);
    }

  private:
    Type **it_ = nullptr;
    difference_type position_ = 0;
  };

  using iterator = common_iterator<false>;
  using const_iterator = common_iterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  iterator begin() noexcept;

  const_iterator begin() const noexcept;

  const_iterator cbegin() const noexcept;

  iterator end() noexcept;

  const_iterator end() const noexcept;

  const_iterator cend() const noexcept;

  reverse_iterator rbegin() noexcept;

  const_reverse_iterator rbegin() const noexcept;

  const_reverse_iterator crbegin() const noexcept;

  reverse_iterator rend() noexcept;

  const_reverse_iterator rend() const noexcept;

  const_reverse_iterator crend() const noexcept;

  void insert(iterator it, const Type& elem);

  void erase(iterator it);

private:
  Type **deque_;
  //first_block_ - первый блок, в котором хранятся значения типа Type
  //first_ - первая позиция в first_block_, в котором хранится значение типа Type
  //last_block_ - последний блок, в котором хранятся значения типа Type
  //last_ - последняя позиция в last_block_, в котором хранится значение типа Type
  //capacity_ - количество блоков
  //size_ - количество элементов в деке
  size_t first_ = 0, first_block_ = 0;
  size_t last_ = 0, last_block_ = 0;
  size_t capacity_ = 0, size_ = 0;
  static const int block = 8;

  void reallocate();

  void allocate();

  void delete_deque() noexcept;

  static const size_t BEGIN = 0;
  static const size_t END = block;
};

template<typename Type>
Deque<Type>::Deque(const Deque<Type>& other) :
  first_(other.first_),
  first_block_(other.first_block_),
  last_(other.last_),
  last_block_(other.last_block_),
  capacity_(other.capacity_) {
  allocate();
  size_t i;
  try {
    for (i = first_block_; i <= last_block_; ++i) {
      size_t begin = 0, end = block;
      if (i == first_block_) begin = first_;
      if (i == last_block_) end = last_;
      for (size_t j = begin; j < end; ++j) {
        new(deque_[i] + j) Type(other.deque_[i][j]);
        ++size_;
      }
    }
  } catch (...) {
    for (size_t ind = 0; ind < size_; ++ind) {
      (*this)[ind].~Type();
    }
    delete_deque();
    throw;
  }
}

template<typename Type>
Deque<Type>::Deque(size_t count, const Type& value) {
  //size_ = count;
  capacity_ = 3 * (count / block + 1);
  first_block_ = capacity_ / 2;
  last_block_ = first_block_ + count / block;
  first_ = 0;
  last_ = count % block;
  allocate();
  size_t i;
  try {
    for (i = first_block_; i <= last_block_; ++i) {
      size_t begin = 0, end = block;
      if (i == first_block_) begin = first_;
      if (i == last_block_) end = last_;
      for (size_t j = begin; j < end; ++j) {
        new(deque_[i] + j) Type(value);
        ++size_;
      }
    }
  }
  catch (...) {
    for (size_t ind = 0; ind < size_; ++ind) {
      (*this)[ind].~Type();
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
void Deque<Type>::swap(Deque& other) noexcept {
if (this != &other) {
std::swap(size_, other.size_);
std::swap(first_, other.first_);
std::swap(first_block_, other.first_block_);
std::swap(last_, other.last_);
std::swap(last_block_, other.last_block_);
std::swap(capacity_, other.capacity_);
std::swap(deque_, other.deque_);
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
  new(deque_[last_block_] + last_) Type(elem);
  ++size_;
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
    new(deque_[first_block_] + first_) Type(elem);
    ++size_;
  } catch (...) {
    first_ = prev_first;
    first_block_ = prev_first_block;
    throw;
  }
}

template<typename Type>
void Deque<Type>::pop_back() noexcept {
if (last_ == 0) {
  --last_block_;
}
last_ = (last_ + block - 1) % block;
deque_[last_block_][last_].~Type();
--size_;
}

template<typename Type>
void Deque<Type>::pop_front() noexcept {
deque_[first_block_][first_].~Type();
if (first_ + 1 == END) {
++first_block_;
}
first_ = (first_ + 1) % block;
--size_;
}

template<typename Type>
void Deque<Type>::reallocate() {
  Type **current = new Type *[3 * capacity_];
  for (size_t i = 0; i < 3 * capacity_; ++i) {
    if (i >= capacity_ && i < 2 * capacity_) {
      current[i] = deque_[i - capacity_];
    } else {
      current[i] = reinterpret_cast<Type *>(new uint8_t[block * sizeof(Type)]);
    }
  }
  delete[] deque_;
  deque_ = current;
  first_block_ += capacity_;
  last_block_ += capacity_;
  capacity_ *= 3;
}

template<typename Type>
void Deque<Type>::allocate() {
  deque_ = new Type *[capacity_];
  for (size_t i = 0; i < capacity_; ++i) {
    deque_[i] = reinterpret_cast<Type *>(new uint8_t[block * sizeof(Type)]);
  }
}

template<typename Type>
void Deque<Type>::delete_deque() noexcept {
  for (size_t i = 0; i < capacity_; ++i) {
    delete[] reinterpret_cast<uint8_t *>(deque_[i]);
  }
  delete[] deque_;
}

template<typename Type>
Deque<Type>::~Deque() noexcept {
for (size_t i = 0; i < size_; ++i) {
(*this)[i].~Type();
}
delete_deque();
}

template<typename Type>
Type& Deque<Type>::operator[](size_t index) noexcept {
if (index < (block - first_)) {
return deque_[first_block_][first_ + index];
}
return deque_[first_block_ + (index - (block - first_)) / block + 1][
(index - (block - first_)) % block];
}

template<typename Type>
const Type& Deque<Type>::operator[](size_t index) const noexcept {
if (index < (block - first_)) return deque_[first_block_][first_ + index];
return deque_[first_block_ + (index - (block - first_)) / block + 1][
(index - (block - first_)) % block];
}

template<typename Type>
typename Deque<Type>::iterator Deque<Type>::begin() noexcept {
return iterator(deque_[first_block_], first_);
}

template<typename Type>
typename Deque<Type>::const_iterator Deque<Type>::begin() const noexcept {
return const_iterator(deque_[first_block_], first_);
};

template<typename Type>
typename Deque<Type>::const_iterator Deque<Type>::cbegin() const noexcept {
return const_iterator(deque_[first_block_], first_);
};

template<typename Type>
typename Deque<Type>::iterator Deque<Type>::end() noexcept {
return iterator(deque_[last_block_], last_);
}

template<typename Type>
typename Deque<Type>::const_iterator Deque<Type>::end() const noexcept {
return const_iterator(deque_[last_block_], last_);
};

template<typename Type>
typename Deque<Type>::const_iterator Deque<Type>::cend() const noexcept {
return const_iterator(deque_[last_block_], last_);
}

template<typename Type>
typename Deque<Type>::reverse_iterator Deque<Type>::rbegin() noexcept {
return reverse_iterator(end());
}

template<typename Type>
typename Deque<Type>::const_reverse_iterator Deque<Type>::rbegin() const noexcept {
return const_reverse_iterator(end());
};

template<typename Type>
typename Deque<Type>::const_reverse_iterator Deque<Type>::crbegin() const noexcept {
return const_reverse_iterator(end());
};

template<typename Type>
typename Deque<Type>::reverse_iterator Deque<Type>::rend() noexcept {
return reverse_iterator(begin());
}

template<typename Type>
typename Deque<Type>::const_reverse_iterator Deque<Type>::rend() const noexcept {
return const_reverse_iterator(begin());
};

template<typename Type>
typename Deque<Type>::const_reverse_iterator Deque<Type>::crend() const noexcept {
return const_reverse_iterator(begin());
};

template<typename Type>
void Deque<Type>::insert(Deque::iterator index, const Type& elem) {
  if (size_ == 0 || index == end()) {
    push_back(elem);
  } else {
    auto copy = *this;
    try {
      iterator current = (--end());
      Type last = *current;
      while (current != index) {
        *(current) = *(current - 1);
        --current;
      }
      *index = elem;
      push_back(last);
    } catch (...) {
      swap(copy);
      throw;
    }
  }

}

template<typename Type>
void Deque<Type>::erase(Deque::iterator index) {
  auto copy = *this;
  try {
    iterator last = end();
    while (last != index) {
      *index = *(index + 1);
      ++index;
    }
    pop_back();
  } catch(...){
    swap(copy);
    throw;
  }
}


#endif //DEQUE_DEQUE_H
