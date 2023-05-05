#ifndef LRU_CACHE_LIST
#define LRU_CACHE_LIST

#include <iostream>
#include <cstring>
#include <string>
#include <cmath>
#include <cstdio>

template<class value_type>
class list {

private:

  class node {
    friend class list;

  private:

    node* pre, * nxt;
    value_type val;

  public:

    node() { pre = nxt = nullptr; }
    node(const value_type& _val, node* _pre = nullptr, node* _nxt = nullptr) :val(_val), pre(_pre), nxt(_nxt) {}
    ~node() {}

  };
  
  node* head, * tail;

  size_t siz;

public:

  list() {
    head = new node; tail = new node;
    head->nxt = tail; tail->pre = head;
    siz = 0;
  }
  ~list() {
    node* it = head, * tmp;
    while (it) {
      tmp = it->nxt;
      delete it; it = tmp;
    }
  }

  class iterator {
    friend class list;

  private:

    node* pointer;

  public:

    iterator(node* _pointer = nullptr) :pointer(_pointer) {}
    iterator(const iterator& other) :pointer(other.pointer) {}
    ~iterator() {}
    iterator& operator ++() {
      pointer = pointer->nxt;
      return *this;
    }
    iterator& operator --() {
      pointer = pointer->pre;
      return *this;
    }
    iterator operator ++(int) {
      iterator tmp(pointer);
      pointer = pointer->nxt;
      return tmp;
    }
    iterator operator --(int) {
      iterator tmp(pointer);
      pointer = pointer->pre;
      return tmp;
    }
    value_type& operator *() {
      return pointer->val;
    }
    bool operator ==(const iterator& other) { return pointer == other.pointer; }
    bool operator !=(const iterator& other) { return pointer != other.pointer; }

  };

  size_t size() { return siz; }

  iterator front() { return iterator(head->nxt); }

  iterator back() { return iterator(tail->pre); }

  iterator end() { return iterator(tail); }

  iterator insert(const value_type& _val) {
    node* Newnode = new node(_val, tail->pre, tail);
    tail->pre->nxt = Newnode;
    tail->pre = Newnode; siz++;
    return iterator(Newnode);
  }

  void del(iterator it) {
    it.pointer->pre->nxt = it.pointer->nxt;
    it.pointer->nxt->pre = it.pointer->pre;
    siz--;
    delete it.pointer;
    it.pointer = nullptr;
  }

  void clear() {
    node* it = head, * tmp;
    it = it->nxt;
    while (it != tail) {
      tmp = it->nxt;
      delete it; it = tmp;
    }
    head->nxt = tail; tail->pre = head;
  }

};

#endif