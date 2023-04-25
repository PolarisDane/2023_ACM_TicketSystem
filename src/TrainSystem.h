#ifndef ACM_TICKETSYSTEM_USERSYSTEM
#define ACM_TICKETSYSTEM_USERSYSTEM

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
    const Time& _st_time, int* trav_time, int* stop_time, const Date& _st_date, const Date& _ed_date, const char& _type)£»

};

class train_pass {

};

class ticket {

};

class TrainSystem {
private:

  BPTree<trainid, int> TrainData;
  BPTree<> PassData;

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
};

#endif