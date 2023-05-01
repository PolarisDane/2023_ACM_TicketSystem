#ifndef LRU_CACHE_HASHMAP
#define LRU_CACHE_HASHMAP

#include <iostream>
#include <cstring>
#include <string>
#include <cmath>
#include <cstdio>

const double LOAD_FACTOR = 0.75;
const int DEFAULT_SIZE = 3000;

template<class Key_Type, class Value_Type, class Hash_Func>
class HashMap {

private:

  class node {
    friend class HashMap;
  private:
    Key_Type key;
    Value_Type val;
    node* nxt;
  public:
    node() {}
    node(const Key_Type& _key, const Value_Type& _val) :key(_key), val(_val), nxt(nullptr) {}
    ~node() {}
  };

  node** tab;
  Hash_Func hash;
  //Equal_Func equal;
  size_t siz;

  //Expansion not done
  void ExpandSize() {}
  //


public:

  HashMap(size_t _size = DEFAULT_SIZE) :siz(_size) {
    tab = new node * [siz];
    for (int i = 0; i < siz; i++) tab[i] = nullptr;
  }

  ~HashMap() {
    for (int i = 0; i < siz; i++) {
      node* now = tab[i], * tmp;
      while (now) {
        tmp = now->nxt;
        delete now; now = tmp;
      }
    }
    delete[] tab;
  }

  void clear() {
    for (int i = 0; i < siz; i++) {
      node* now = tab[i], * tmp;
      while (now) {
        tmp = now->nxt;
        delete now; now = tmp;
      }
      tab[i] = nullptr;
    }
  }

  Value_Type& operator [](const Key_Type& _key) {
    size_t index = hash(_key) % siz;
    node* now = tab[index];
    while (now) {
      if (now->key == _key) return now->val;
      now = now->nxt;
    }
    now = new node; now->key = _key; now->nxt = tab[index];
    tab[index] = now;
    return now->val;
  }

  void erase(const Key_Type& _key) {
    size_t index = hash(_key) % siz;
    node* now = tab[index], * pre = nullptr;
    while (now) {
      if (now->key == _key) {
        if (!pre) tab[index] = now->nxt;
        else pre->nxt = now->nxt;
        delete now;
        return;
      }
      pre = now; now = now->nxt;
    }
  }

  bool find(const Key_Type& _key) {
    size_t index = hash(_key) % siz;
    node* now = tab[index];
    while (now) {
      if (now->key == _key) return true;
      now = now->nxt;
    }
    return false;
  }

};

#endif
