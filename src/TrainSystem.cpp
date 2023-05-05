#include "TrainSystem.h"

Train::Train(const trainid& _trainID, const int& _stationNum, const int& _seatNum, station* _stations, int* _price,
  const Time& _st_time, int* trav_time, int* stop_time, const Date& _st_date, const Date& _ed_date, const char& _type)
  :trainID(_trainID), stationNum(_stationNum), seatNum(_seatNum), st_time(_st_time), st_date(_st_date), ed_date(_ed_date), type(_type) {
  st_time.date = st_date;
  for (int i = 1; i <= stationNum; i++)
    stations[i] = _stations[i];
  price[1] = 0;
  for (int i = 2; i <= stationNum; i++) price[i] = price[i - 1] + _price[i - 1];
  int tim = 0; arriv_time[1] = leave_time[1] = 0;
  for (int i = 2; i <= stationNum; i++) {
    tim += trav_time[i - 1];
    arriv_time[i] = tim;
    tim += stop_time[i - 1];
    leave_time[i] = tim;
  }
  release = false;
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
  stationCnt = stationNum;
  for (int i = 1; i < stationNum; i++) remain[i] = seatNum;
  for (int i = 1; i <= block_cnt; i++) {
    block[i] = (stationNum - 1) > ((i - 1) * block_len) ? seatNum : 0;
    tag[i] = 0;
  }
}

int train_ticket::query_ticket(int st, int ed) {
  int ans = 2e9; ed--;
  //for (int i = st; i <= ed; i++) ans = std::min(ans, remain[i]);
  int id1 = get_id(st), id2 = get_id(ed);
  if (id1 == id2) {
    for (int i = st; i <= ed; i++) ans = std::min(ans, remain[i] + tag[id1]);
    return ans;
  }
  for (int i = id1 + 1; i <= id2 - 1; i++)
    ans = std::min(ans, block[i]);
  for (int i = st; i <= id1 * block_len; i++)
    ans = std::min(ans, remain[i] + tag[id1]);
  for (int i = (id2 - 1) * block_len + 1; i <= ed; i++)
    ans = std::min(ans, remain[i] + tag[id2]);
  return ans;
}

void train_ticket::modify_ticket(int st, int ed, const int& cnt) {
  ed--;
  //for (int i = st; i <= ed; i++) remain[i] += cnt;
  int id1 = get_id(st), id2 = get_id(ed);
  if (id1 == id2) {
    for (int i = st; i <= ed; i++) remain[i] += cnt;
    block[id1] = 2e9;
    for (int i = (id1 - 1) * block_len + 1; i <= std::min(id1 * block_len, stationCnt - 1); i++)
      block[id1] = std::min(block[id1], remain[i] + tag[id1]);
    return;
  }
  for (int i = id1 + 1; i <= id2 - 1; i++) {
    block[i] += cnt; tag[i] += cnt;
  }
  for (int i = st; i <= id1 * block_len; i++)
    remain[i] += cnt;
  block[id1] = 2e9;
  for (int i = (id1 - 1) * block_len + 1; i <= id1 * block_len; i++)
    block[id1] = std::min(block[id1], remain[i] + tag[id1]);
  for (int i = (id2 - 1) * block_len + 1; i <= ed; i++)
    remain[i] += cnt;
  block[id2] = 2e9;
  for (int i = (id2 - 1) * block_len + 1; i <= std::min(id2 * block_len, stationCnt - 1); i++)
    block[id2] = std::min(block[id2], remain[i] + tag[id2]);
}

int TrainSystem::add_train(const trainid& newID, const int& newstationNum, const int& newseatNum, station* _stations, int* _price, const Time& new_st_time,
  int* new_trav_time, int* new_stop_time, const Date& new_st_Date, const Date& new_ed_Date, const char& newType) {
  if (TrainData.data.count(newID)) throw(exceptions("Identical train ID"));

  Train newTrain(newID, newstationNum, newseatNum, _stations, _price, new_st_time,
    new_trav_time, new_stop_time, new_st_Date, new_ed_Date, newType);

  TrainData.data.insert(newID, TrainData.dataCnt);
  TrainData.write(TrainData.dataCnt++, newTrain);

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

  train_ticket ticket(train.stationNum, train.seatNum);
  for (int i = 0; i <= train.ed_date - train.st_date; i++) {
    TicketData.data.insert(std::make_pair(train.trainID, i), TicketData.dataCnt);
    TicketData.write(TicketData.dataCnt++, ticket);
  }

  for (int i = 1; i <= train.stationNum; i++) {
    train_pass pass(train.trainID, train.st_date, train.ed_date, train.st_time, train.arriv_time[i], train.leave_time[i], train.price[i], i);
    PassData.data.insert(train.stations[i], PassData.dataCnt);
    PassData.write(PassData.dataCnt++, pass);
  }

  return 0;
}

int TrainSystem::query_train(const trainid& trainID, const Date& date) {
  auto res = TrainData.data.find(trainID);
  if (res.empty()) throw(exceptions("Train doesn't exist"));
  Train train; TrainData.read(res[0], train);

  if (date<train.st_date || date>train.ed_date) throw(exceptions("Train not found"));

  int days = date - train.st_date;

  std::cout << train.trainID << " " << train.type << std::endl;
  for (int i = 1; i <= train.stationNum; i++) {
    std::cout << train.stations[i] << " ";
    if (i == 1) std::cout << "xx-xx xx:xx";
    else std::cout << train.st_time + days * 1440 + train.arriv_time[i];
    std::cout << " -> ";
    if (i == train.stationNum) std::cout << "xx-xx xx:xx ";
    else std::cout << train.st_time + days * 1440 + train.leave_time[i] << " ";
    std::cout << train.price[i] << " ";
    if (i == train.stationNum) std::cout << "x" << std::endl;
    else {
      if (!train.release) std::cout << train.seatNum << std::endl;
      else {
        auto res = TicketData.data.find(std::make_pair(train.trainID, days));
        train_ticket ret; TicketData.read(res[0], ret);
        std::cout << ret.remain[i] + ret.tag[train_ticket::get_id(i)] << std::endl;
      }
    }
  }

  return 0;
}

int TrainSystem::query_ticket(const station& st_sta, const station& ed_sta, const Date& date, int opt) {
  vector<trip_ticket> ans;
  auto res1 = PassData.data.find(st_sta);
  auto res2 = PassData.data.find(ed_sta);
  vector<train_pass> res_st;
  train_pass tmp; mp.clear();
  for (int i = 0; i < res1.size(); i++) {
    PassData.read(res1[i], tmp);
    res_st.push_back(tmp);
    mp[tmp.trainID] = i;
  }
  for (int i = 0; i < res2.size(); i++) {
    PassData.read(res2[i], tmp);
    if (mp.find(tmp.trainID)) {
      int p1 = mp[tmp.trainID];
      if (res_st[p1].pos >= tmp.pos) continue;

      int days = date - (res_st[p1].st_time + res_st[p1].leave_time).date;
      if (days < 0 || days>res_st[p1].ed_date - res_st[p1].st_date) continue;

      auto ret = TicketData.data.find(std::make_pair(tmp.trainID, days));
      train_ticket ticket; TicketData.read(ret[0], ticket);
      trip_ticket trip(tmp.trainID, res_st[p1].st_time + days * 1440 + res_st[p1].leave_time,
        res_st[p1].st_time + days * 1440 + tmp.arriv_time, tmp.priceSum - res_st[p1].priceSum, ticket.query_ticket(res_st[p1].pos, tmp.pos));
      ans.push_back(trip);
    }
  }
  //int p1 = 0, p2 = 0;
  //for (; p1 < res1.size(); p1++) {
  //  while (p2 < res2.size() && res1[p1] >= res2[p2]) {
  //    std::cout << "pos:" << res1[p1] << " " << res2[p2] << std::endl;
  //    if (res_st[p1].trainID == res_ed[p2].trainID) {
  //      if (res_st[p1].pos < res_ed[p2].pos) {

  //        int days = date - res_st[p1].st_date;
  //        trainid trainID = res_st[p1].trainID;

  //        if (date > res_st[p1].ed_date || date < res_st[p1].st_date) continue;
  //        int st = res_st[p1].pos, ed = res_ed[p2].pos;

  //        auto ret = TicketData.data.find(std::make_pair(trainID, days));
  //        train_ticket ticket; TicketData.read(ret[0], ticket);

  //        trip_ticket trip(trainID, res_st[p1].st_time + days * 1440 + res_st[p1].leave_time,
  //          res_st[p1].st_time + days * 1440 + res_ed[p2].arriv_time, res_ed[p2].priceSum - res_st[p1].priceSum, ticket.query_ticket(st, ed));
  //        ans.push_back(trip);
  //      }
  //    }
  //    p2++;
  //  }
  //}
  if (opt == 0) sort<trip_ticket, cmpByTime>(ans, 0, ans.size() - 1);
  else if (opt == 1) sort<trip_ticket, cmpByCost>(ans, 0, ans.size() - 1);

  std::cout << ans.size() << std::endl;
  for (int i = 0; i < ans.size(); i++) {
    std::cout << ans[i].trainID << " " << st_sta << " " << ans[i].leave_time << " -> ";
    std::cout << ed_sta << " " << ans[i].arriv_time << " " << ans[i].price << " " << ans[i].remainSeatNum << std::endl;
  }

  return 0;
}

int TrainSystem::query_transfer(const station& st_sta, const station& ed_sta, const Date& date, int opt) {
  bool flag = false;
  transfer_ticket ans; station inter_sta;
  auto res_st = PassData.data.find(st_sta);
  auto res_ed = PassData.data.find(ed_sta);
  HashMap<station, int, Stringhash> transfer_mp;

  train_pass pass_st;

  vector<train_pass> pass_ed;
  for (int i = 0; i < res_ed.size(); i++) {
    PassData.read(res_ed[i], pass_st);
    pass_ed.push_back(pass_st);
  }

  for (int p1 = 0; p1 < res_st.size(); p1++) {
    PassData.read(res_st[p1], pass_st);

    int st_days = date - (pass_st.st_time + pass_st.leave_time).date;
    if (st_days < 0 || st_days > pass_st.ed_date - pass_st.st_date) continue;

    auto res = TrainData.data.find(pass_st.trainID);
    Train train_st; TrainData.read(res[0], train_st);

    transfer_mp.clear();
    for (int i = pass_st.pos + 1; i <= train_st.stationNum; i++) transfer_mp[train_st.stations[i]] = i;

    for (int p2 = 0; p2 < pass_ed.size(); p2++) {
      if (pass_st.trainID == pass_ed[p2].trainID) continue;

      res = TrainData.data.find(pass_ed[p2].trainID);
      Train train_ed; TrainData.read(res[0], train_ed);

      for (int i = 1; i <= pass_ed[p2].pos - 1; i++) {
        int inter_sta_pos = transfer_mp[train_ed.stations[i]];//车站在 train_st 中的位置
        if (inter_sta_pos == 0) continue;

        Time inter_arriv_time = train_st.st_time + st_days * 1440 + train_st.arriv_time[inter_sta_pos];
        int ed_days = inter_arriv_time.date - (train_ed.st_time + train_ed.leave_time[i]).date;
        if (ed_days < 0) ed_days = 0;
        //std::cout << "ed_days:" << ed_days << std::endl;
        Time inter_leave_time = train_ed.st_time + ed_days * 1440 + train_ed.leave_time[i];
        //std::cout << "inter_arriv_time:" << inter_arriv_time << " inter_leave_time: " << inter_leave_time << std::endl;
        if (inter_leave_time < inter_arriv_time) ed_days++;

        if (ed_days > pass_ed[p2].ed_date - pass_ed[p2].st_date) continue;

        res = TicketData.data.find(std::make_pair(train_st.trainID, st_days));
        train_ticket st_ticket; TicketData.read(res[0], st_ticket);
        int rem_seat_st = st_ticket.query_ticket(pass_st.pos, inter_sta_pos);

        res = TicketData.data.find(std::make_pair(train_ed.trainID, ed_days));
        train_ticket ed_ticket; TicketData.read(res[0], ed_ticket);
        int rem_seat_ed = ed_ticket.query_ticket(i, pass_ed[p2].pos);

        transfer_ticket nowAns = std::make_pair(
          trip_ticket(train_st.trainID, train_st.st_time + st_days * 1440 + train_st.leave_time[pass_st.pos], train_st.st_time + st_days * 1440 + train_st.arriv_time[inter_sta_pos],
            train_st.price[inter_sta_pos] - train_st.price[pass_st.pos], rem_seat_st),
          trip_ticket(train_ed.trainID, train_ed.st_time + ed_days * 1440 + train_ed.leave_time[i], train_ed.st_time + ed_days * 1440 + train_ed.arriv_time[pass_ed[p2].pos],
            train_ed.price[pass_ed[p2].pos] - train_ed.price[i], rem_seat_ed)
        );
        
        //inter_sta = train_ed.stations[i];
        //std::cout << nowAns.first.trainID << " " << st_sta << " " << nowAns.first.leave_time << " -> ";
        //std::cout << inter_sta << " " << nowAns.first.arriv_time << " " << nowAns.first.price << " " << nowAns.first.remainSeatNum << std::endl;
        //std::cout << nowAns.second.trainID << " " << inter_sta << " " << nowAns.second.leave_time << " -> ";
        //std::cout << ed_sta << " " << nowAns.second.arriv_time << " " << nowAns.second.price << " " << nowAns.second.remainSeatNum << std::endl;
        if (!flag) {
          inter_sta = train_ed.stations[i];
          ans = nowAns;
          flag = true;
        }
        else {
          if (opt == 0) {
            cmpByTime comp;
            if (comp(nowAns, ans)) {
              inter_sta = train_ed.stations[i];
              ans = nowAns;
            }
          }
          else if (opt == 1) {
            cmpByCost comp;
            if (comp(nowAns, ans)) {
              inter_sta = train_ed.stations[i];
              ans = nowAns;
            }
          }
        }
      }
    }
  }

  if (flag) {
    std::cout << ans.first.trainID << " " << st_sta << " " << ans.first.leave_time << " -> ";
    std::cout << inter_sta << " " << ans.first.arriv_time << " " << ans.first.price << " " << ans.first.remainSeatNum << std::endl;
    std::cout << ans.second.trainID << " " << inter_sta << " " << ans.second.leave_time << " -> ";
    std::cout << ed_sta << " " << ans.second.arriv_time << " " << ans.second.price << " " << ans.second.remainSeatNum << std::endl;
  }
  else std::cout << "0" << std::endl;

  return 0;
}

