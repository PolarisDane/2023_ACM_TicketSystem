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

train_ticket::train_ticket(const int& stationNum, const int& seatNum) {
  for (int i = 1; i < stationNum; i++) remain[i] = seatNum;
  for (int i = 1; i <= block_cnt; i++) {
    block[i] = seatNum;
    tag[i] = 0;
  }
}

static int train_ticket::get_id(int x) {
  return (x - 1) / block_len;
}

int train_ticket::query_ticket(const int& st, const int& ed) {
  int ans = 2e9;
  int id1 = get_id(st), id2 = get_id(ed);
  for (int i = id1 + 1; i <= id2 - 1; i++)
    ans = std::min(ans, block[i]);
  for (int i = st; i <= id1 * block_len; i++)
    ans = std::min(ans, remain[i] + tag[id1]);
  for (int i = (id2 - 1) * block_len + 1; i <= ed; i++)
    ans = std::min(ans, remain[i] + tag[id2]);
  return ans;
}

void train_ticket::modify_ticket(const int& st, const int& ed, const int& cnt) {
  int id1 = get_id(st), id2 = get_id(ed);
  for (int i = id1 + 1; i <= id2 - 1; i++) {
    block[i] += cnt; tag[i] += cnt;
  }
  for (int i = st; i <= id1 * block_len; i++)
    remain[i] += cnt;
  for (int i = (id2 - 1) * block_len + 1; i <= ed; i++)
    remain[i] += cnt;
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

  train_ticket ticket(newstationNum, newseatNum);
  for (int i = 1; i <= new_ed_Date - new_st_Date + 1; i++)
    TicketData.insert(std::make_pair(newID, i), ticket);

  return 0;
}

int TrainSystem::delete_train(const trainid& trainID) {
  auto res = TrainData.find(trainID);
  if (res.empty()) throw(exceptions("Train doesn't exist"));
  int curPos = res[0];
  Train train; readTrain(curPos, train);

  if (train.release) throw(exceptions("Train already released"));

  TrainData.del(trainID);

  train_ticket ticket(train.stationNum, train.seatNum);
  for (int i = 1; i <= train.ed_date - train.st_date + 1; i++)
    TicketData.del(std::make_pair(train.trainID, i), ticket);

  return 0;
}

int TrainSystem::release_train(const trainid& trainID) {
  auto res = TrainData.find(trainID);
  if (res.empty()) throw(exceptions("Train doesn't exist"));
  int curPos = res[0];
  Train train; readTrain(curPos, train);

  if (train.release) throw(exceptions("Train already released"));

  train.release = true;
  writeTrain(curPos, train);

  for (int i = 1; i <= train.stationNum - 1; i++) {
    PassData.insert(train.stations[i], train_pass(train.trainID, train.st_date, train.ed_date, train.arriv_time[i], train.st_time[i - 1], train.price[i + 1], i + 1));
  }

  return 0;
}

int TrainSystem::query_train(const trainid& trainID, const Date& date) {
  auto res = TrainData.find(trainID);
  if (res.empty()) throw(exceptions("Train doesn't exist"));
  int curPos = res[0];
  Train train; readTrain(curPos, train);

  std::cout << train.trainID << " " << train.type << std::endl;
  for (int i = 1; i <= train.stationNum; i++) {
    std::cout << train.stations[i] << " " << train.arriv_time[i] << " -> " << train.leave_time << " ";
    std::cout << train.price[i] << " ";
    auto res = TicketData.find(std::make_pair(train.trainID, i));
    std::cout << res[0].remain[i] + res[0].tag[res[0].get_id(i)] << std::endl;
  }

  return 0;
}

int TrainSystem::query_ticket(const station& st_sta, const station& ed_sta, const Date& date, int opt) {
  vector<trip_ticket> ans;
  auto res_st = PassData.find(st_sta);
  auto res_ed = PassData.find(ed_sta);
  int p1 = 0, p2 = 0;
  for (; p1 < res_st.size(); p1++) {
    while (p2 < res_ed.size() && res_st[p1].trainID > res_ed[p2].trainID) {
      p2++;
      if (res_st[p1].trainID == res_ed[p2].trainID) {
        if (res_st[p1].pos < res_ed[p2].pos) {
          auto trainID = res_st[p1].trainID;
          int st = res_st[p1].pos, ed = res_ed[p2].pos;
          if (date > res_st[p1].ed_date || date < res_st[p1].st_date) continue;
          auto ret = TicketData.find(std::make_pair(trainID, date - res_st[p1].st_date + 1));
          train_ticket ticket = ret[0];
          trip_ticket trip(train.trainID, train.st_time + train.leave_time[st],
            train.st_time + train.arriv_time[ed], train.price[ed] - train.price[st - 1], ticket.query_ticket(st, ed));
          ans.push_back(trip);
        }
      }
    }
  }
  //for (int i = 0; i < res.size(); i++) {
  //  auto ret = TrainData.find(res[i].trainID);
  //  Train train = ret[0];
  //  int st = res[i].pos;
  //  for (int ed = st + 1; ed <= train.stationNum; ed++) {
  //    if (ed_sta == train.stations[ed]) {
  //      auto ret = TicketData.find(std::make_pair(train.trainID, date - train.st_date + 1));
  //      train_ticket ticket = ret[0];
  //      trip_ticket trip(train.trainID, train.st_time + train.leave_time[st],
  //        train.st_time + train.arriv_time[ed], train.price[ed] - train.price[st - 1], ticket.query_ticket(st, ed));
  //      ans.push_back(trip);
  //      break;
  //    }
  //  }
  //}
  //slow but good-looking
  if (opt == 0) sort<trip_ticket, cmpByTime>(ans, 0, ans.size());
  else if (opt == 1) sort<trip_ticket, cmpByCost>(ans, 0, ans.size());

  std::cout << abs.size() << std::endl;
  for (int i = 0; i < ans.size(); i++) {
    std::cout << ans[i].trainID << " " << st_sta << " " << ans[i].leave_time << " -> ";
    std::cout << ed_sta << " " << ans[i].arriv_time << " " << ans[i].price << " " << ans[i].remainSeatNum << std::endl;
  }
  return 0;
}

int TrainSystem::query_transfer(const station& st_sta, const station& ed_sta, const Date& date, int opt) {
  bool flag = false;
  transfer_ticket ans; station inter_sta;

  if (flag) {
    std::cout << ans.first.trainID << " " << st_sta << " " << ans.first.leave_time << " -> ";
    std::cout << inter_sta << " " << ans.first.arriv_time << " " << ans.first.price << " " << ans.first.remainSeatNum << std::endl;
    std::cout << ans.second.trainID << " " << inter_sta << " " << ans.second.leave_time << " -> ";
    std::cout << ed_sta << " " << ans.second.arriv_time << " " << ans.second.price << " " << ans.second.remainSeatNum << std::endl;
  }
  else std::cout << "0" << std::endl;
  return 0;
}

