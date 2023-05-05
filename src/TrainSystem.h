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
#include "utils.h"
#include "FileSystem.h"

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
  Time st_time;
  int arriv_time, leave_time;
  int priceSum;
  int pos;
  train_pass() = default;
  ~train_pass() = default;
  train_pass(const train_pass& other) = default;

  train_pass(const trainid& _trainID, const Date& _st_date, const Date& _ed_date, const Time& _st_time,const int& _arriv_time, const int& _leave_time, const int& _priceSum, const int& _pos)
    :trainID(_trainID), st_date(_st_date), ed_date(_ed_date), st_time(_st_time), arriv_time(_arriv_time), leave_time(_leave_time), priceSum(_priceSum), pos(_pos) {}
};

const int block_len = std::sqrt(MAX_STATION_NUM);//10

const int block_cnt = MAX_STATION_NUM / block_len + 1;//11

class train_ticket {
public:
  int stationCnt;
  int remain[MAX_STATION_NUM];
  int tag[block_cnt + 1];
  int block[block_cnt + 1];

  train_ticket() = default;
  train_ticket(const int& stationNum, const int& seatNum);
  ~train_ticket() = default;

  int query_ticket(int st, int ed);

  void modify_ticket(int st, int ed, const int& cnt);

  static int get_id(int x) {
    return (x - 1) / block_len + 1;
  }

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
    else return t1.trainID <= t2.trainID;
  }
  bool operator ()(const transfer_ticket& t1, const transfer_ticket& t2)const {
    int tim1 = t1.second.arriv_time - t1.first.leave_time;
    int tim2 = t2.second.arriv_time - t2.first.leave_time;
    int price1 = t1.first.price + t1.second.price;
    int price2 = t2.first.price + t2.second.price;
    if (tim1 != tim2) return tim1 < tim2;
    else if (price1 != price2) return price1 < price2;
    else if (t1.first.trainID != t2.first.trainID) return t1.first.trainID < t2.first.trainID;
    else return t1.second.trainID <= t2.second.trainID;
  }
};

class cmpByCost {
public:
  bool operator ()(const trip_ticket& t1, const trip_ticket& t2)const {
    if (t1.price != t2.price) return t1.price < t2.price;
    else return t1.trainID <= t2.trainID;
  }
  bool operator ()(const transfer_ticket& t1, const transfer_ticket& t2)const {
    int tim1 = t1.second.arriv_time - t1.first.leave_time;
    int tim2 = t2.second.arriv_time - t2.first.leave_time;
    int price1 = t1.first.price + t1.second.price;
    int price2 = t2.first.price + t2.second.price;
    if (price1 != price2) return price1 < price2;
    else if (tim1 != tim2) return tim1 < tim2;
    else if (t1.first.trainID != t2.first.trainID) return t1.first.trainID < t2.first.trainID;
    else return t1.second.trainID <= t2.second.trainID;
  }
};

class TrainSystem {

  friend class TicketSystem;

private:

  FileSystem<trainid, Train> TrainData;
  FileSystem<std::pair<trainid, int>, train_ticket> TicketData;
  FileSystem<station, train_pass> PassData;

public:

  HashMap<trainid, int, Stringhash> mp = HashMap<trainid, int, Stringhash>(3000);

  TrainSystem() :TrainData("Train"), TicketData("Ticket"), PassData("Pass") {}
  ~TrainSystem() = default;

  int add_train(const trainid& newID, const int& newstationNum, const int& newseatNum, station* _stations, int* _price, const Time& new_st_time,
    int* new_trav_time, int* new_stop_time, const Date& new_st_Date, const Date& new_ed_Date, const char& newType);
  int delete_train(const trainid& trainID);
  int release_train(const trainid& trainID);
  int query_train(const trainid& trainID, const Date& date);
  int query_ticket(const station& st_sta, const station& ed_sta, const Date& date, int opt);
  int query_transfer(const station& st_sta, const station& ed_sta, const Date& date, int opt);
};

#endif