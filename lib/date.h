#ifndef ACM_TICKETSYSTEM_DATE
#define ACM_TICKETSYSTEM_DATE

#include <iostream>

const int day_count[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

const int day_sum[13] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };

class Date {
public:
  int month, day;
  Date() = default;
  ~Date() = default;

  Date(int _month, int _day) :month(_month), day(_day) {}
  Date(const Date& other) :month(other.month), day(other.day) {}

  bool operator <(const Date& other)const {
    if (month != other.month) return month < other.month;
    else return day < other.day;
  }
  bool operator >(const Date& other)const {
    if (month != other.month) return month > other.month;
    else return day > other.day;
  }
  bool operator ==(const Date& other)const {
    return month == other.month && day == other.day;
  }
  bool operator !=(const Date& other)const {
    return !((*this) == other);
  }
  bool operator <=(const Date& other)const {
    return (*this) < other || (*this) == other;
  }
  bool operator >=(const Date& other)const {
    return (*this) > other || (*this) == other;
  }

  Date operator +(int days) {
    Date tmp((*this)); tmp.day += days;
    while (tmp.day > day_count[tmp.month]) {
      tmp.day -= day_count[tmp.month];
      tmp.month++;
    }
    return tmp;
  }

  Date operator -(int days) {
    Date tmp((*this)); tmp.day -= days;
    while (tmp.day <= 0) {
      tmp.month--;
      tmp.day += day_count[tmp.month];
    }
    return tmp;
  }

  Date& operator +=(int days) {
    day += days;
    while (day > day_count[month]) {
      day -= day_count[month];
      month++;
    }
    return (*this);
  }

  Date& operator -=(int days) {
    day -= days;
    while (day <= 0) {
      month--;
      day += day_count[month];
    }
    return (*this);
  }

  int operator -(const Date& other) {
    int day1 = day_sum[month - 1] + day;
    int day2 = day_sum[other.month - 1] + other.day;
    return day1 - day2;
  }

  friend std::ostream& operator <<(std::ostream& os, const Date& date) {
    if (date.month < 10) os << "0";
    os << date.month << "-";
    if (date.day < 10) os << "0";
    os << date.day;
    return os;
  }

};

class Time {
public:
  Date date;
  int hour, min;

  Time() = default;
  ~Time() = default;

  Time(int _month = 1, int _day = 1, int _hour, int _min) :date(_month, _day), hour(_hour), min(_min) {}
  Time(const Time& other) :date(other.date), hour(other.hour), min(other.min) {}

  bool operator <(const Time& other)const {
    if (date != other.date) return date < other.date;
    else if (hour != other.hour) return hour < other.hour;
    else return min < other.min;
  }
  bool operator >(const Time& other)const {
    if (date != other.date) return date > other.date;
    else if (hour != other.hour) return hour > other.hour;
    else return min > other.min;
  }
  bool operator ==(const Time& other)const {
    return date == other.date && hour == other.hour && min == other.min;
  }
  bool operator !=(const Time& other)const {
    return !((*this) == other);
  }
  bool operator <=(const Time& other)const {
    return (*this) < other || (*this) == other;
  }
  bool operator >=(const Time& other)const {
    return (*this) > other || (*this) == other;
  }

  Time operator +(int mins) {
    Time tmp((*this)); tmp.min += mins;
    if (tmp.min >= 60) {
      tmp.hour += tmp.min / 60; tmp.min %= 60;
      if (tmp.hour >= 24) {
        tmp.date += tmp.hour / 24; tmp.hour %= 24;
      }
    }
    return tmp;
  }

  Time operator -(int mins) {
    Time tmp((*this)); tmp.min -= mins;
    if (tmp.min <= 0) {
      tmp.hour -= (-tmp.min + 59) / 60; tmp.min = (60 + tmp.min % 60);
      if (tmp.hour <= 0) {
        tmp.date -= (-tmp.hour + 23) / 24; tmp.hour = (24 + tmp.hour % 24);
      }
    }
    return tmp;
  }

  Time& operator +=(int mins) {
    min += mins;
    if (min >= 60) {
      hour += min / 60; min %= 60;
      if (hour >= 24) {
        date += hour / 24; hour %= 24;
      }
    }
    return (*this);
  }

  Time& operator -=(int mins) {
    min -= mins;
    if (min <= 0) {
      hour -= (-min + 59) / 60; min = (60 + min % 60);
      if (hour <= 0) {
        date -= (-hour + 23) / 24; hour = (24 + hour % 24);
      }
    }
    return (*this);
  }

  int operator -(const Time& other) {
    int ans = (date - other.date) * 24 * 60;
    ans += hour * 60 + min - other.hour * 60 - other.min;
  }

  friend std::ostream& operator <<(std::ostream& os, const Time& tim) {
    os << tim.date << " ";
    if (tim.hour < 10) os << "0";
    os << tim.hour << ":";
    if (tim.min < 10) os << "0";
    os << tim.min;
    return os;
  }

};

#endif