#ifndef STRING_H
#define STRING_H

#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>

static std::hash<std::string> HashStr;

template<size_t size>
class String {
public:
  char str[size];
  String() {
    memset(str, 0, sizeof(str));
  }
  template<size_t _size>
  String(const String<_size>& s) {
    strcpy(str, s.str);
  }
  String(const std::string& s) {
    strcpy(str, s.c_str());
  }
  String(const char* s) {
    strcpy(str, s);
  }
  bool empty()const {
    return !strlen(str);
  }
  operator std::string()const {
    return std::string(str);
  }
  String& operator =(const std::string& s) {
    strcpy(str, s.c_str());
    return (*this);
  }
  String& operator =(const char* s) {
    strcpy(str, s);
    return (*this);
  }
  bool operator <(const String& other)const {
    return strcmp(str, other.str) < 0;
  }
  bool operator >(const String& other)const {
    return strcmp(str, other.str) > 0;
  }
  bool operator <=(const String& other)const {
    return strcmp(str, other.str) <= 0;
  }
  bool operator >=(const String& other)const {
    return strcmp(str, other.str) >= 0;
  }
  bool operator ==(const String& other)const {
    return strcmp(str, other.str) == 0;
  }
  bool operator !=(const String& other)const {
    return strcmp(str, other.str) != 0;
  }
};

template<size_t size>
std::istream& operator >>(std::istream& is, String<size>& str) {
  std::string s; is >> s;
  strcpy(str.str, s.c_str());
  return is;
}

template<size_t size>
std::ostream& operator <<(std::ostream& os, const String<size>& str) {
  for (int i = 0; i < std::strlen(str.str); i++) os << str.str[i];
  return os;
}


class Stringhash {
public:
  template<size_t size>
  size_t operator () (const String<size>& str) {
    return HashStr(str);
  }
};

template<size_t size>
size_t getHash(const String<size>& str) {
  return HashStr(str);
}

using username = String<21>;
using userpassword = String<31>;
using userrealname = String<21>;
using usermail = String<31>;
using trainid = String<21>;
using station = String<41>;


#endif