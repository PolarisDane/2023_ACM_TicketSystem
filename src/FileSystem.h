#ifndef ACM_TICKETSYSTEM_FILESYSTEM
#define ACM_TICKETSYSTEM_FILESYSTEM

#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <cmath>
#include <fstream>
#include "vector.h"
#include "BPT.h"

template<class key_type, class val_type>
class FileSystem {
public:
  std::fstream file;
  BPTree<key_type, int> data;
  int dataCnt;
public:
  FileSystem(const std::string& file_name) :data(file_name) {
    if (!data.size()) {
      dataCnt = 0;
      std::fstream create;
      create.open(file_name + ".data", std::ios::out); create.close();
      file.open(file_name + ".data", std::ios::binary | std::ios::out | std::ios::in);
    }
    else {
      file.open(file_name + ".data", std::ios::binary | std::ios::out | std::ios::in);
      file.seekg(0);
      file.read(reinterpret_cast<char*>(&dataCnt), sizeof(int));
    }
  }
  ~FileSystem() {
    file.seekp(0);
    file.write(reinterpret_cast<char*>(&dataCnt), sizeof(int));
    file.close();
  }
  void read(const int& pos, val_type& val) {
    file.seekg(pos * sizeof(val_type) + sizeof(int));
    file.read(reinterpret_cast<char*>(&val), sizeof(val_type));
  }
  void write(const int& pos, val_type& val) {
    file.seekp(pos * sizeof(val_type) + sizeof(int));
    file.write(reinterpret_cast<char*>(&val), sizeof(val_type));
  }
};

#endif