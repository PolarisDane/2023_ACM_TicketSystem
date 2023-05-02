#ifndef ACM_TICKETSYSTEM_PARSER
#define ACM_TICKETSYSTEM_PARSER

#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <cmath>
#include "TrainSystem.h"
#include "MyString.h"
#include "date.h"
#include "vector.h"

void ReadLine(const std::string& str,std::string* buf) {
  int pos = 0, cnt = 0;
  for (int i = 0; i < str.length(); i++) {
    if (str[i] == ' ') {
      if (pos != i) buf[++cnt] = str.substr(pos, i - pos);
      pos = i + 1;
    }
  }
  buf[++cnt] = str.substr(pos);
  buf[0] = std::to_string(cnt);
}

void parse_station(station* stations,const std::string& str) {
  int pos = 0, cnt = 0;
  for (int i = 0; i < str.length(); i++) {
    if (str[i] == '|') {
      stations[++cnt] = str.substr(pos, i - pos);
      pos = i + 1;
    }
  }
  stations[++cnt] = str.substr(pos);
}

void parse_num(int* _array,const std::string& str) {
  int val = 0, cnt = 0;
  for (int i = 0; i < str.length(); i++) {
    if (str[i] == '|') {
      _array[++cnt] = val; val = 0;
    }
    else val = val * 10 + str[i] - '0';
  }
  _array[++cnt] = val;
}

void parse_time_point(Time& tim, const std::string& str) {
  tim.hour = (str[0] - '0') * 10 + (str[1] - '0');
  tim.min = (str[3] - '0') * 10 + (str[4] - '0');
}

void parse_date(Date& date, const std::string& str) {
  date.month = (str[0] - '0') * 10 + (str[1] - '0');
  date.day = (str[3] - '0') * 10 + (str[4] - '0');
}

void parse_st_ed_date(Date& new_st_Date, Date& new_ed_Date, const std::string& str) {
  parse_date(new_st_Date, str);
  parse_date(new_ed_Date, str.substr(6));
}

#endif