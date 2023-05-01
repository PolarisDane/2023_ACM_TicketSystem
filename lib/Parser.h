#ifndef ACM_TICKETSYSTEM_PARSER
#define ACM_TICKETSYSTEM_PARSER

#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <cmath>
#include "MyString.h"
#include "vector.h"

void ReadLine(const std::string& str,std::string* buf) {
  int pos = 0, cnt = 0;
  for (int i = 0; i < str.length(); i++) {
    if (str[i] == ' ') {
      if (pos != i) buf[++cnt] = str.substr(pos, i - pos);
      pos = i + 1;
    }
  }
  if (pos <= str.length() - 1) {
    buf[++cnt] = str.substr(pos, str.length() - pos + 1);
  }
  buf[0] = std::to_string(cnt);
}

void parse_station() {

}

void parse_price() {

}

void parse_time_point() {

}

void parse_time() {

}

void parse_date() {

}

void parse_st_ed_date() {

}

#endif