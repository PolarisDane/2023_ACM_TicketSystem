#ifndef ACM_TICKETSYSTEM_VECTOR
#define ACM_TICKETSYSTEM_VECTOR

#include <iostream>
#include "exceptions.h"

template<class T>
class vector
{
public:
  class const_iterator;
  class iterator
  {
    friend class vector;
  private:
    int pos;
    T* beg;
  public:
    iterator(int p = 0, T* begin = nullptr) {
      pos = p;
      beg = begin;
    }
    iterator operator+(const int& n) const
    {
      iterator newIterator(pos + n, beg);
      return newIterator;
    }
    iterator operator-(const int& n) const
    {
      iterator newIterator(pos - n, beg);
      return newIterator;
    }
    int operator-(const iterator& rhs) const
    {
      if (beg != rhs.beg) throw(exceptions("Invalid iterator"));
      return pos - rhs.pos;
    }
    iterator& operator+=(const int& n)
    {
      pos += n;
      return (*this);
    }
    iterator& operator-=(const int& n)
    {
      pos -= n;
      return (*this);
    }
    iterator operator++(int) {
      iterator newIterator = (*this);
      pos++;
      return newIterator;
    }
    iterator& operator++() {
      pos++;
      return (*this);
    }
    iterator operator--(int) {
      iterator newIterator = (*this);
      pos--;
      return newIterator;
    }
    iterator& operator--() {
      pos--;
      return (*this);
    }
    T& operator*() const {
      return *(beg + pos);
    }
    bool operator==(const iterator& rhs) const {
      return beg == rhs.beg && pos == rhs.pos;
    }
    bool operator==(const const_iterator& rhs) const {
      return beg == rhs.beg && pos == rhs.pos;
    }
    bool operator!=(const iterator& rhs) const {
      return !((*this) == rhs);
    }
    bool operator!=(const const_iterator& rhs) const {
      return !((*this) == rhs);
    }
  };
  class const_iterator
  {
    friend class vector;
  public:
    const_iterator(int p = 0, T* begin = nullptr) {
      pos = p;
      beg = begin;
    }
    const_iterator operator+(const int& n) const
    {
      const_iterator newIterator(pos + n, beg);
      return newIterator;
    }
    iterator operator-(const int& n) const
    {
      const_iterator newIterator(pos - n, beg);
      return newIterator;
    }
    int operator-(const const_iterator& rhs) const
    {
      if (beg != rhs.beg) throw(exceptions("Invalid iterator"));
      return pos - rhs.pos;
    }
    const_iterator& operator+=(const int& n)
    {
      pos += n;
      return (*this);
    }
    const_iterator& operator-=(const int& n)
    {
      pos -= n;
      return (*this);
    }
    const_iterator operator++(int) {
      const_iterator newIterator = (*this);
      pos++;
      return newIterator;
    }
    const_iterator& operator++() {
      pos++;
      return (*this);
    }
    const_iterator operator--(int) {
      const_iterator newIterator = (*this);
      pos--;
      return newIterator;
    }
    const_iterator& operator--() {
      pos--;
      return (*this);
    }
    T operator*() const {
      return *(beg + pos);
    }
    bool operator==(const iterator& rhs) const {
      return beg == rhs.beg && pos == rhs.pos;
    }
    bool operator==(const const_iterator& rhs) const {
      return beg == rhs.beg && pos == rhs.pos;
    }
    bool operator!=(const iterator& rhs) const {
      return !((*this) == rhs);
    }
    bool operator!=(const const_iterator& rhs) const {
      return !((*this) == rhs);
    }
  private:
    int pos;
    T* beg;
  };
private:
  T* head;
  int capacity, siz;
  void doubleCapacity() {
    capacity <<= 1;
    T* tmp = (T*)malloc(capacity * sizeof(T));
    for (int i = 0; i < siz; i++) {
      new (tmp + i) T(head[i]);
      head[i].~T();
    }
    free(head);
    head = tmp;
  }
public:
  vector() {
    capacity = 4;
    siz = 0;
    head = (T*)malloc(capacity * sizeof(T));
  }
  vector(const vector& other) {
    capacity = other.capacity; siz = other.siz;
    head = (T*)malloc(capacity * sizeof(T));
    for (int i = 0; i < siz; i++) new (head + i) T(other.head[i]);
  }
  ~vector() {
    for (int i = 0; i < siz; i++) head[i].~T();
    free(head); head = nullptr;
    capacity = siz = 0;
  }
  vector& operator=(const vector& other) {
    if (head == other.head) return (*this);
    for (int i = 0; i < siz; i++) head[i].~T();
    free(head);
    capacity = other.capacity; siz = other.siz;
    head = (T*)malloc(capacity * sizeof(T));
    for (int i = 0; i < siz; i++) new (head + i) T(other.head[i]);
    return (*this);
  }
  T& at(const size_t& pos) {
    if (pos < 0 || pos >= siz) throw(exceptions("Index out of bound"));
    return head[pos];
  }
  const T& at(const size_t& pos) const {
    if (pos < 0 || pos >= siz) throw(exceptions("Index out of bound"));
    return head[pos];
  }
  T& operator[](const size_t& pos) {
    if (pos < 0 || pos >= siz) throw(exceptions("Index out of bound"));
    return head[pos];
  }
  const T& operator[](const size_t& pos) const {
    if (pos < 0 || pos >= siz) throw(exceptions("Index out of bound"));
    return head[pos];
  }
  const T& front() const {
    if (!siz) throw(exceptions("Container is empty"));
    return head[0];
  }
  const T& back() const {
    if (!siz) throw(exceptions("Container is empty"));
    return head[siz - 1];
  }
  iterator begin() {
    return iterator(0, head);
  }
  const_iterator cbegin() const {
    return const_iterator(0, head);
  }
  iterator end() {
    return iterator(siz, head);
  }
  const_iterator cend() const {
    return const_iterator(siz, head);
  }
  bool empty() const {
    return !siz;
  }
  size_t size() const {
    return siz;
  }
  void clear() {
    for (int i = 0; i < siz; i++) head[i].~T();
    siz = 0;
  }
  iterator insert(iterator pos, const T& value) {
    if (pos.pos > siz) throw(exceptions("Index out of bound"));
    if (capacity == siz) doubleCapacity();
    for (int i = siz; i > pos.pos; i--) new (head + i) T(head[i - 1]);
    head[pos.pos] = value; siz++;
    return pos;
  }
  iterator insert(const size_t& ind, const T& value) {
    if (ind > siz || ind < 0) throw(exceptions("Index out of bound"));
    if (capacity == siz) doubleCapacity();
    for (int i = siz; i > ind; i--) new (head + i) T(head[i - 1]);
    head[ind] = value; siz++;
    return iterator(ind, head);
  }
  iterator erase(iterator pos) {
    if (pos.pos > siz) throw(exceptions("Index out of bound"));
    head[pos.pos].~T();
    for (int i = pos.pos; i < siz; i++) head[i] = head[i + 1];
    siz--;
    return pos;
  }
  iterator erase(const size_t& ind) {
    if (ind >= siz || ind < 0) throw(exceptions("Index out of bound"));
    head[ind].~T();
    for (int i = ind; i < siz; i++) head[i] = head[i + 1];
    siz--;
    return iterator(ind, head);
  }
  void push_back(const T& value) {
    if (capacity == siz) doubleCapacity();
    new (head + siz) T(value);
    siz++;
  }
  void pop_back() {
    if (!siz) throw(exceptions("Container is empty"));
    head[--siz].~T();
  }
};

#endif