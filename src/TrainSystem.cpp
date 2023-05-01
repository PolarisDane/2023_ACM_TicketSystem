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

Time Train::get_arrive_time(const Date& date, const int& sta_pos) {
  Time ans(st_time);
  ans.date = date;
  ans += arriv_time[sta_pos];
  return ans;
}

Time Train::get_leave_time(const Date& date, const int& sta_pos) {
  Time ans(st_time);
  ans.date = date;
  ans += leave_time[sta_pos];
  return ans;
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

int TrainSystem::add_train(const trainid& newID, const int& newstationNum, const int& newseatNum, station* _stations, int* _price, const Time& new_st_time,
  int* new_trav_time, int* new_stop_time, const Date& new_st_Date, const Date& new_ed_Date, const char& newType) {
  if (TrainData.data.count(newID)) throw(exceptions("Identical train ID"));

  Train newTrain(newType, newID, newstationNum, newseatNum, _stations, _price, new_st_time,
    new_trav_time, new_stop_time, newseatNum, new_st_Date, new_ed_Date);

  TrainData.data.insert(newID, TrainData.dataCnt);
  TrainData.write(TrainData.datacCnt++, newTrain);

  train_ticket ticket(newstationNum, newseatNum);
  for (int i = 1; i <= new_ed_Date - new_st_Date + 1; i++) {
    TicketData.data.insert(std::make_pair(newID, i), TicketData.dataCnt);
    TicketData.write(TicketData.dataCnt++, ticket);
  }

  return 0;
}

int TrainSystem::delete_train(const trainid& trainID) {
  auto res = TrainData.data.find(trainID);
  if (res.empty()) throw(exceptions("Train doesn't exist"));
  Train train; TrainData.read(res[0], train);

  if (train.release) throw(exceptions("Train already released"));

  TrainData.data.del(trainID, res[0]);

  return 0;
}

int TrainSystem::release_train(const trainid& trainID) {
  auto res = TrainData.data.find(trainID);
  if (res.empty()) throw(exceptions("Train doesn't exist"));
  Train train; TrainData.read(res[0], train);

  if (train.release) throw(exceptions("Train already released"));

  train.release = true;
  TrainData.write(res[0], train);

  for (int i = 1; i <= train.stationNum - 1; i++) {
    PassData.data.insert(train.stations[i], PassData.dataCnt);
    PassData.write(PassData.dataCnt++, train_pass(train.trainID, train.st_date, train.ed_date, train.arriv_time[i], train.st_time[i - 1], train.price[i + 1], i + 1));
  }

  return 0;
}

int TrainSystem::query_train(const trainid& trainID, const Date& date) {
  auto res = TrainData.data.find(trainID);
  if (res.empty()) throw(exceptions("Train doesn't exist"));
  Train train; TrainData.read(res[0], train);

  if (date<train.st_date || date>train.ed_date) throw(exceptions("Train not found"));

  int days = date - train.st_date + 1;

  std::cout << train.trainID << " " << train.type << std::endl;
  for (int i = 1; i <= train.stationNum; i++) {
    std::cout << train.stations[i] << " " << train.arriv_time[i] << " -> " << train.leave_time << " ";
    std::cout << train.price[i] << " ";
    auto res = TicketData.find(std::make_pair(train.trainID, days));
    train_ticket ret; TicketData.read(res[0], ret);
    std::cout << ret.remain[i] + ret.tag[res[0].get_id(i)] << std::endl;
  }

  return 0;
}

int TrainSystem::query_ticket(const station& st_sta, const station& ed_sta, const Date& date, int opt) {
  vector<trip_ticket> ans;

  auto res = PassData.data.find(st_sta);
  vector<train_pass> res_st, res_ed;
  train_pass tmp;
  for (int i = 0; i < res1.size(); i++) {
    PassData.read(res[i], tmp);
    res_st.push_back(tmp);
  }
  res = PassData.data.find(ed_sta);
  for (int i = 0; i < res1.size(); i++) {
    PassData.read(res[i], tmp);
    res_ed.push_back(tmp);
  }

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
  //until test done
  //bool flag = false;
  //transfer_ticket ans; station inter_sta;
  //auto res_st = PassData.find(st_sta);
  //auto res_ed = PassData.find(ed_sta);
  //HashMap<station, int, Stringhash> mp;
  //for (int p1 = 0; p1 < res_st.size(); i++) {
  //  if (date > res_st[p1].ed_date || date < res_st[p1].st_date) continue;
  //  Train train_st;
  //  auto res = TrainData.find(res_st[p1].trainID);
  //  int curPos = res[0];
  //  Train train_st; readTrain(curPos, train_st);
  //  mp.clear();
  //  for (int i = res_st[p1].pos + 1; i <= train_st.stationNum; i++) mp[train_st.stations[i]] = i;
  //  for (int p2 = 0; p2 < res_ed.size(); i++) {
  //    if (st.trainID == ed.trainID) continue;
  //    res = TrainData.find(res_ed[p2].trainID);
  //    curPos = res[0];
  //    Train train_ed; readTrain(curPos, train_ed);
  //    for (int i = 1; i <= res_ed[p2].pos - 1; i++) {
  //      int inter_sta_pos = mp[train_ed.stations[i]];//车站在 train_st 中的位置
  //      Time inter_arriv_time = train_st.get_arrive_time(date, inter_sta_pos);
  //      inter_arriv_time -= train_ed.leave_time[i];
  //      if (inter_arriv_time < train_ed.get_leave_time(train_ed.st_date, 1) || inter_arriv_time > train_ed.get_leave_time(train_ed.ed_date, 1)) continue;
  //      if ();

  //      res = TicketData.find(std::make_pair(train_st.trainID, date - train_st.st_date + 1));

  //      int rem_seat_st=res

  //      transfer_ticket nowAns(
  //        trip_ticket(train_st.trainID, train_st.leave_time[res_st[p1].pos], train_st.arriv_time[inter_sta_pos],
  //          train_st.price[inter_sta_pos] - train_st.price[res_st[p1].pos - 1], rem_seat_st), ,
  //        trip_ticket(train_ed.trainID, train_ed.leave_time[], train_ed.arriv_time[res_ed[p2].pos],
  //          train_ed.price[res_ed[p2].pos] - train_ed.price[i - 1], rem_seat_ed);
  //      );

  //      if (!flag) {
  //        ans = nowAns;
  //        flag = true;
  //      }
  //      else {
  //        if (opt == 0) {
  //          if (cmpByTime(nowAns, ans)) ans = nowAns;
  //        }
  //        else if (opt == 1) {
  //          if (cmpByCost(nowAns, ans)) ans = nowAns;
  //        }
  //      }
  //    }
  //  }
  //}

  //if (flag) {
  //  std::cout << ans.first.trainID << " " << st_sta << " " << ans.first.leave_time << " -> ";
  //  std::cout << inter_sta << " " << ans.first.arriv_time << " " << ans.first.price << " " << ans.first.remainSeatNum << std::endl;
  //  std::cout << ans.second.trainID << " " << inter_sta << " " << ans.second.leave_time << " -> ";
  //  std::cout << ed_sta << " " << ans.second.arriv_time << " " << ans.second.price << " " << ans.second.remainSeatNum << std::endl;
  //}
  //else std::cout << "0" << std::endl;

  return 0;
}

