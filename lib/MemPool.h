#ifndef ACM_TICKETSYSTEM_MEMPOOL
#define ACM_TICKETSYSTEM_MEMPOOL

#include <iostream>
#include <cstdio>
#include <fstream>
#include <cstring>
#include <string>
#include "vector.h"
#include "exceptions.h"

template<class T>
class MemPool {
private:
  int cnt;
  vector<T> Mem;
  std::fstream mem_file;
public:
  MemPool(const std::string& file_name) {
    mem_file.open(file_name + ".mem", std::ios::binary | std::ios::out | std::ios::in);
    if (mem_file.good()) {
      T x;
      mem_file.read(reinterpret_cast<char*>(&cnt), sizeof(int));
      for (int i = 0; i < cnt; i++) {
        mem_file.read(reinterpret_cast<char*>(&x), sizeof(T));
        Mem.push_back(x);
      }
    }
    else {
      cnt = 0;
      std::fstream create;
      create.open(file_name + ".mem", std::ios::out);
      create.close();
      mem_file.open(file_name + ".mem", std::ios::binary | std::ios::out | std::ios::in);
      Mem.clear();
    }
  }
  ~MemPool() {
    mem_file.write(reinterpret_cast<char*>(&cnt), sizeof(int));
    for (int i = 0; i < cnt; i++)
      mem_file.write(reinterpret_cast<char*>(&Mem[i]), sizeof(T));
    mem_file.close();
  }
  bool empty() {
    return Mem.empty();
  }
  T back() {
    T tmp = Mem[--cnt];
    Mem.pop_back();
    return tmp;
  }
  void insert(const T& _val) {
    Mem.push_back(_val);
    cnt++;
  }
  void clear() {
    Mem.clear();
    cnt = 0;
  }
};

#endif