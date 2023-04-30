#ifndef ACM_TICKETSYSTEM_TRAINSYSTEM
#define ACM_TICKETSYSTEM_TRAINSYSTEM

#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <cmath>
#include "MyString.h"
#include "hashmap.h"
#include "vector.h"
#include "BPT.h"
#include "date.h"
#include "UserSystem.h"
#include "utils.h"

const int MAX_STATION_NUM = 101;

class Train {
public:
  bool release;

  char type;
  trainid trainID;
  int stationNum;
  station stations[MAX_STATION_NUM];
  int price[MAX_STATION_NUM];

  int arriv_time[MAX_STATION_NUM], leave_time[MAX_STATION_NUM];
  
  int seatNum;

  Date st_date, ed_date;

  Time st_time;

  Train() = default;
  Train(const Train& other) = default;
  ~Train() = default;

  Train(const trainid& _trainID, const int& _stationNum, const int& _seatNum, station* _stations, int* _price,
    const Time& _st_time, int* trav_time, int* stop_time, const Date& _st_date, const Date& _ed_date, const char& _type);

  Time get_arrive_time(const Date& date, const int& sta_pos);

  Time get_leave_time(const Date& date, const int& sta_pos);

};

class train_pass {
public:
  trainid trainID;
  Date st_date, ed_date;
  Time arriv_time;
  int stop_time;
  int priceSum;
  int pos;
  train_pass() = default;
  ~train_pass() = default;
  train_pass(const train_pass& other) = default;

  train_pass(const trainid& _trainID, const Date& _st_date, const Date& _ed_date, const Time& _arrive_time, const int& _stop_time, const int& _priceSum, const int& _pos)
    :trainID(_trainID), st_date(_st_date), ed_date(_ed_date), arrive_time(_arrive_time), stop_time(_stop_time), priceSum(_priceSum), pos(_pos) {}
};

const int block_len = std::sqrt(MAX_STATION_NUM);

const int block_cnt = MAX_STATION_NUM / block_len + 1;

class train_ticket {
public:
  int remain[MAX_STATION_NUM];
  int tag[block_cnt];
  int block[block_cnt];

  train_ticket() = default;
  train_ticket(const int& stationNum, const int& seatNum);
  ~train_ticket() = default;

  void query_ticket(const int& st, const int& ed);

  void modify_ticket(const int& st, const int& ed, const int& cnt);

  static int int get_id(int x);

};

class trip_ticket {
public:
  trainid trainID;
  Time leave_time, arriv_time;
  int price;
  int remainSeatNum;
  trip_ticket() = default;
  trip_ticket(const trainid& _trainID, const Time& _leave_time, const Time& _arriv_time, const int& _price, const int& _remainSeatNum)
    :trainID(_trainID), leave_time(_leave_time), arriv_time(_arriv_time), price(_price), remainSeatNum(_remainSeatNum) {}
  ~trip_ticket() = default;
};

using transfer_ticket = std::pair<trip_ticket, trip_ticket>;

class cmpByTime {
public:
  bool operator ()(const trip_ticket& t1, const trip_ticket& t2)const {
    int tim1 = t1.arriv_time - t1.leave_time;
    int tim2 = t2.arriv_time - t2.leave_time;
    if (tim1 != tim2) return tim1 < tim2;
    else return t1.trainID < t2.trainID;
  }
};

class cmpByCost {
  bool operator ()(const trip_ticket& t1, const trip_ticket& t2)const {
    if (t1.price != t2.price) return t1.price < t2.price;
    else return t1.trainID < t2.trainID;
  }
};

class TrainSystem {
private:

  BPTree<trainid, int> TrainData;
  BPTree<std::pair<trainid, int>, train_ticket> TicketData;
  BPTree<station, train_pass> PassData;

  std::fstream train_file;
  int train_cnt;

public:

  TrainSystem() {}
  ~TrainSystem() {}

  void readTrain(const int& pos, Train& p);
  void writeTrain(const int& pos, Train& p);
  int add_train(const trainid& newID, const int& newstationNum, const int& newseatNum, station* _stations, int* _price, const Time& new_st_time,
    int* new_trav_time, int* new_stop_time, const int& newseatNum, const Date& new_st_Date, const Date& new_ed_Date, const char& newType);
  int delete_train(const trainid& trainID);
  int release_train(const trainid& trainID);
  int query_train(const trainid& trainID, const Date& date);
  int query_ticket(const station& st_sta, const station& ed_sta, const Date& date, int opt);
  int query_transfer(const station& st_sta, const station& ed_sta, const Date& date, int opt);
};

#endif