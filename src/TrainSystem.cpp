#include "TrainSystem.h"

Train::Train(const trainid& _trainID, const int& _stationNum, const int& _seatNum, station* _stations, int* _price,
  const Time& _st_time, int* trav_time, int* stop_time, const Date& _st_date, const Date& _ed_date, const char& _type)
  :trainID(_trainID), stationNum(_stationNum), seatNum(_seatNum), st_time(_st_time), st_date(_st_date), ed_date(_ed_date), type(_type) {
  for (int i = 1; i <= stationNum; i++)
    stations[i] = _stations[i];
  price[1] = 0;
  for (int i = 2; i <= stationNum; i++) price[i] = price[i - 1] + _price[i - 1];
  int tim = 0;
  for (int i = 2; i <= stationNum; i++) {
    tim += trav_tim[i - 1];
    arriv_time[i] = tim;
    tim += stop_tim[i - 1];
    leave_time[i] = tim;
  }
}

TrainSystem::TrainSystem() :TrainData("Train") {
  if (!TrainData.size()) {
    train_cnt = 0;
    std::fstream create;
    create.open("Train.data", std::ios::out); create.close();
    train_file.open("Train.data", std::ios::binary | std::ios::out | std::ios::in);
  }
  else {
    train_file.open("Train.data", std::ios::binary | std::ios::out | std::ios::in);
    train_file.seekg(0);
    train_file.read(reinterpret_cast<char*>(&train_cnt), sizeof(int));
  }
}

TrainSystem::~TrainSystem() {
  train_file.seekp(0);
  train_file.write(reinterpret_cast<char*>(&train_cnt), sizeof(int));
  train_file.close();
}

void TrainSystem::readTrain(const int& pos, Train& p) {
  train_file.seekg(pos * sizeof(Train) + sizeof(int));
  train_file.read(reinterpret_cast<char*>(&p), sizeof(Train));
}
void TrainSystem::writeTrain(const int& pos, Train& p) {
  train_file.seekp(pos * sizeof(Train) + sizeof(int));
  train_file.write(reinterpret_cast<char*>(&p), sizeof(Train));
}

int TrainSystem::add_train(const trainid& newID, const int& newstationNum, const int& newseatNum, station* _stations, int* _price, const Time& new_st_time,
  int* new_trav_time, int* new_stop_time, const int& newseatNum, const Date& new_st_Date, const Date& new_ed_Date, const char& newType) {
  if (trainData.count(newID)) throw(exceptions("Identical train ID"));

  Train newTrain(newType, newID, newstationNum, newseatNum, _stations, _price, new_st_time,
    new_trav_time, new_stop_time, newseatNum, new_st_Date, new_ed_Date);

  TrainData.insert(newID, train_cnt);
  writeTrain(train_cnt++, newTrain);
  return 0;
}

int TrainSystem::delete_train(const trainid& trainID) {
  auto res = TrainData.find(trainID);
  if (res.empty()) throw(exceptions("Train doesn't exist"));
  int curPos = res[0];
  Train train; readTrain(curPos, train);

  if (train.release) throw(exceptions("Train already released"));

  TrainData.del(trainID);

  return 0;
}

int TrainSystem::release_train(const trainid& trainID) {
  auto res = TrainData.find(trainID);
  if (res.empty()) throw(exceptions("Train doesn't exist"));
  int curPos = res[0];
  Train train; readTrain(curPos, train);

  if (train.release) throw(exceptions("Train already released"));

  train.release = true;
  wroteTrain(curPos, train);



  return 0;
}