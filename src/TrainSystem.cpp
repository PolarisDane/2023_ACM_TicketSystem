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
  if (TrainData.count(getHash(newID))) throw(exceptions("Identical train ID"));

  Train newTrain(newID, newstationNum, newseatNum, _stations, _price, new_st_time,
    new_trav_time, new_stop_time, new_st_Date, new_ed_Date, newType);

  TrainData.insert(getHash(newID), newTrain);

  return 0;
}

int TrainSystem::delete_train(const trainid& trainID) {
  size_t trainHash = getHash(trainID);

  auto res = TrainData.find(trainHash);
  if (!res) throw(exceptions("Train doesn't exist"));
  Train train; TrainData.readVal(res, train);

  if (train.release) throw(exceptions("Train already released"));

  TrainData.del(trainHash);

  return 0;
}

int TrainSystem::release_train(const trainid& trainID) {
  size_t trainHash = getHash(trainID);

  auto res = TrainData.find(trainHash);
  if (!res) throw(exceptions("Train doesn't exist"));
  Train train; TrainData.readVal(res, train);

  if (train.release) throw(exceptions("Train already released"));

  train.release = true;
  TrainData.writeVal(res, train);

  train_ticket ticket(train.stationNum, train.seatNum);
  for (int i = 0; i <= train.ed_date - train.st_date; i++)
    TicketData.insert(std::make_pair(trainHash, i), ticket);

  for (int i = 1; i <= train.stationNum; i++) {
    train_pass pass(train.trainID, train.st_date, train.ed_date, train.st_time, train.arriv_time[i], train.leave_time[i], train.price[i], i);
    PassData.insert(std::make_pair(getHash(train.stations[i]), trainHash), pass);
  }

  return 0;
}

int TrainSystem::query_train(const trainid& trainID, const Date& date) {
  auto res = TrainData.find(getHash(trainID));
  if (!res) throw(exceptions("Train doesn't exist"));
  Train train; TrainData.readVal(res, train);

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
        auto res = TicketData.find(std::make_pair(getHash(train.trainID), days));
        train_ticket ret; TicketData.readVal(res, ret);
        std::cout << ret.remain[i] + ret.tag[train_ticket::get_id(i)] << std::endl;
      }
    }
  }

  return 0;
}

int TrainSystem::query_ticket(const station& st_sta, const station& ed_sta, const Date& date, int opt) {
  vector<trip_ticket> ans;

  auto iter_st_1 = PassData.lower_bound(std::make_pair(getHash(st_sta), 0));
  auto iter_st_2 = PassData.lower_bound(std::make_pair(getHash(st_sta), UINT64_MAX));
  auto iter_ed_1 = PassData.lower_bound(std::make_pair(getHash(ed_sta), 0));
  auto iter_ed_2 = PassData.lower_bound(std::make_pair(getHash(ed_sta), UINT64_MAX));

  for (; iter_st_1 != iter_st_2; ++iter_st_1) {
    size_t train_hash = iter_st_1.get_key().second;
    while (iter_ed_1 != iter_ed_2 && train_hash > iter_ed_1.get_key().second) ++iter_ed_1;
    if (iter_ed_1 == iter_ed_2) break;
    if (train_hash == iter_ed_1.get_key().second) {

      train_pass st_pass = iter_st_1.get_val();
      train_pass ed_pass = iter_ed_1.get_val();

      //std::cout << "st_pos:" << st_pass.pos << " ed_pos:" << ed_pass.pos << std::endl;

      if (st_pass.pos >= ed_pass.pos) continue;

      int days = date - (st_pass.st_time + st_pass.leave_time).date;
      if (days < 0 || days > st_pass.ed_date - st_pass.st_date) continue;

      auto ret = TicketData.find(std::make_pair(getHash(st_pass.trainID), days));
      train_ticket ticket; TicketData.readVal(ret, ticket);
      trip_ticket trip(st_pass.trainID, st_pass.st_time + days * 1440 + st_pass.leave_time,
        st_pass.st_time + days * 1440 + ed_pass.arriv_time, ed_pass.priceSum - st_pass.priceSum, ticket.query_ticket(st_pass.pos, ed_pass.pos));
      ans.push_back(trip);
    }
  }

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

  auto iter_st_1 = PassData.lower_bound(std::make_pair(getHash(st_sta), 0));
  auto iter_st_2 = PassData.lower_bound(std::make_pair(getHash(st_sta), UINT64_MAX));
  auto iter_ed_1 = PassData.lower_bound(std::make_pair(getHash(ed_sta), 0));
  auto iter_ed_2 = PassData.lower_bound(std::make_pair(getHash(ed_sta), UINT64_MAX));

  vector<train_pass> pass_st, pass_ed;
  for (; iter_st_1 != iter_st_2; ++iter_st_1) 
    pass_st.push_back(iter_st_1.get_val());
  for (; iter_ed_1 != iter_ed_2; ++iter_ed_1)
    pass_ed.push_back(iter_ed_1.get_val());
  HashMap<station, int, Stringhash> transfer_mp;

  for (int p1 = 0; p1 < pass_st.size(); p1++) {

    int st_days = date - (pass_st[p1].st_time + pass_st[p1].leave_time).date;
    if (st_days < 0 || st_days > pass_st[p1].ed_date - pass_st[p1].st_date) continue;

    auto res = TrainData.find(getHash(pass_st[p1].trainID));
    Train train_st; TrainData.readVal(res, train_st);

    transfer_mp.clear();
    for (int i = pass_st[p1].pos + 1; i <= train_st.stationNum; i++) transfer_mp[train_st.stations[i]] = i;

    for (int p2 = 0; p2 < pass_ed.size(); p2++) {
      if (pass_st[p1].trainID == pass_ed[p2].trainID) continue;

      res = TrainData.find(getHash(pass_ed[p2].trainID));
      Train train_ed; TrainData.readVal(res, train_ed);

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

        res = TicketData.find(std::make_pair(getHash(train_st.trainID), st_days));
        train_ticket st_ticket; TicketData.readVal(res, st_ticket);
        int rem_seat_st = st_ticket.query_ticket(pass_st[p1].pos, inter_sta_pos);

        res = TicketData.find(std::make_pair(getHash(train_ed.trainID), ed_days));
        train_ticket ed_ticket; TicketData.readVal(res, ed_ticket);
        int rem_seat_ed = ed_ticket.query_ticket(i, pass_ed[p2].pos);

        transfer_ticket nowAns = std::make_pair(
          trip_ticket(train_st.trainID, train_st.st_time + st_days * 1440 + train_st.leave_time[pass_st[p1].pos], train_st.st_time + st_days * 1440 + train_st.arriv_time[inter_sta_pos],
            train_st.price[inter_sta_pos] - train_st.price[pass_st[p1].pos], rem_seat_st),
          trip_ticket(train_ed.trainID, train_ed.st_time + ed_days * 1440 + train_ed.leave_time[i], train_ed.st_time + ed_days * 1440 + train_ed.arriv_time[pass_ed[p2].pos],
            train_ed.price[pass_ed[p2].pos] - train_ed.price[i], rem_seat_ed)
        );
        
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

