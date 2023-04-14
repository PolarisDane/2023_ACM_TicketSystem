#ifndef STRING_H
#define STRING_H

#include <iostream>
#include <cstring>
#include <string>

class String {
public:
  char str[65];
  String() {
    memset(str, 0, sizeof(str));
  }
  String(const std::string& s) {
    strcpy(str, s.c_str());
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

class Stringhash {
public:
  size_t operator () (const String& str) {
    size_t hash = 0;
    for (int i = 0; i < 64; i++) {
      hash = hash << 7 ^ str.str[i];
    }
    return (hash & 0x7FFFFFFF);
  }
};

std::istream& operator >>(std::istream& is, String& str) {
  std::string s; is >> s;
  strcpy(str.str, s.c_str());
  return is;
}

std::ostream& operator <<(std::ostream& os, const String& str) {
  for (int i = 0; i < std::strlen(str.str); i++) os << str.str[i];
  return os;
}

#endif