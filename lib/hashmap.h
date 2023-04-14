#ifndef LRU_CACHE_HASHMAP
#define LRU_CACHE_HASHMAP

#include <iostream>
#include <cstring>
#include <string>
#include <cmath>
#include <cstdio>

const double LOAD_FACTOR = 0.75;
const int DEFAULT_SIZE = 4096;

template<class Key_Type, class Value_Type, class Hash_Func>
class HashMap {

private:

  class node {
    friend class HashMap;
  private:
    Key_Type key;
    Value_Type val;
    unsigned long long hashVal;
    node* nxt;
  public:
    node() {}
    ~node() {}
  };

  node** tab;
  Hash_Func hash;
  //Equal_Func equal;
  size_t siz;
  size_t capacity;
  
  void ExpandSize() {
    capacity <<= 1;
    node** newtab;
    newtab = new node * [capacity];
    for (int i = 0; i < capacity; i++) newtab = nullptr;
    for (int i = 0; i < (capacity >> 1); i++) {
      auto ls = tab[i];
      if (ls == nullptr) continue;
      if (ls[0].nxt == nullptr) {
        newtab[i] = ls;
        continue;
      }
      node* low_head = nullptr, * high_head = nullptr, * now = ls, * nxtNode;
      while (now) {
        nxtNode = now->nxt;
        if (now->hashVal & (capacity >> 1)) {
          now->nxt = high_head; high_head = now;
        }
        else {
          now->nxt = low_head; low_head = now;
        }
      }
      newtab[i] = low_head; newtab[i + (capacity >> 1)] = high_head;
    }
    delete[] tab;
    tab = newtab;
  }
  //Waiting to be fixed, adding a well functional flexible hashmap


public:

  HashMap(size_t _size = DEFAULT_SIZE) :capacity(_size) {
    tab = new node * [capacity];
    for (int i = 0; i < capacity; i++) tab[i] = nullptr;
  }

  ~HashMap() {
    for (int i = 0; i < capacity; i++) {
      node* now = tab[i], * tmp;
      while (now) {
        tmp = now->nxt;
        delete now; now = tmp;
      }
    }
    delete[] tab;
  }

  Value_Type& operator [](const Key_Type& _key) {
    unsigned long long _hashVal = hash(_key);
    size_t index = _hashVal & (capacity - 1);
    node* now = tab[index];
    while (now) {
      if (now->key == _key) return now->val;
      now = now->nxt;
    }
    if (siz == capacity * LOAD_FACTOR) ExpandSize();
    now = new node; now->key = _key; now->nxt = tab[index]; now->hashVal = _hashVal;
    tab[index] = now;
    return now->val;
  }

  void erase(const Key_Type& _key) {
    size_t index = hash(_key) & (capacity - 1);
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
    size_t index = hash(_key) & (capacity - 1);
    node* now = tab[index];
    while (now) {
      if (now->key == _key) return true;
      now = now->nxt;
    }
    return false;
  }

};

#endif