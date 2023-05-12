#include "TicketSystem.h"

int TicketSystem::buy_ticket(const username& UserName, const trainid& trainID, const Date& date, const int& ticketNum,
  const station& st_sta, const station& ed_sta, const bool& type) {
  if (!UserSys.UserStat[UserName]) throw(exceptions("Not loginned"));

  auto res = TrainSys.TrainData.find(getHash(trainID));
  if (!res) throw(exceptions("Train doesn't exist"));
  Train train; TrainSys.TrainData.readVal(res, train);

  if (!train.release) throw(exceptions("Train not released"));

  if (ticketNum > train.seatNum) throw(exceptions("Expected tickets exceeds total tickets"));

  int st = 0, ed = 0;
  for (int i = 1; i <= train.stationNum; i++) {
    if (train.stations[i] == st_sta) st = i;
    if (train.stations[i] == ed_sta) ed = i;
  }
  if (!st || !ed || st >= ed) throw(exceptions("Ticket not found"));

  int days = date - (train.st_time + train.leave_time[st]).date;
  if (days > train.ed_date - train.st_date || days < 0) throw(exceptions("Train doesn't exist"));

  int ticket_pos = TrainSys.TicketData.find(std::make_pair(getHash(trainID), days));
  train_ticket ticket; TrainSys.TicketData.readVal(ticket_pos, ticket);

  user_ticket userTicket(UserName, 1, trainID, train.st_date + days, st_sta, ed_sta,
    ticketNum, train.st_time + days * 1440 + train.arriv_time[ed], train.st_time + days * 1440 + train.leave_time[st], train.price[ed] - train.price[st], days, st, ed);

  res = UserTicketCnt.find(getHash(UserName));
  int userTicketCnt = 0;
  if (!res) {
    UserTicketCnt.insert(getHash(UserName), userTicketCnt);
    res = UserTicketCnt.valCnt;
  }
  else UserTicketCnt.readVal(res, userTicketCnt);

  if (ticket.query_ticket(st, ed) >= ticketNum) {
    UserTicketData.insert(std::make_pair(getHash(UserName), ++userTicketCnt), userTicket);

    ticket.modify_ticket(st, ed, -ticketNum);
    TrainSys.TicketData.writeVal(ticket_pos, ticket);

    std::cout << (long long)ticketNum * (train.price[ed] - train.price[st]) << std::endl;
  }
  else {
    if (type) {
      userTicket.status = 0;
      UserTicketData.insert(std::make_pair(getHash(UserName), ++userTicketCnt), userTicket);

      pend_ticket pend(UserName, trainID, st, ed, ticketNum, UserTicketData.valCnt);
      PendData.insert(std::make_pair(std::make_pair(getHash(trainID), days), PendData.valCnt), pend);

      std::cout << "queue" << std::endl;
    }
    else throw(exceptions("Ticket not enough"));
  }

  UserTicketCnt.writeVal(res, userTicketCnt);

  return 0;
}

int TicketSystem::query_order(const username& UserName) {
  if (!UserSys.UserStat[UserName]) throw(exceptions("Not loginned"));

  auto iter_st = UserTicketData.lower_bound(std::make_pair(getHash(UserName), 0));
  auto iter_ed = UserTicketData.lower_bound(std::make_pair(getHash(UserName), INT32_MAX));

  vector<user_ticket> res;

  for (; iter_st != iter_ed; ++iter_st)
    res.push_back(iter_st.get_val());

  std::cout << res.size() << std::endl;
  for (int i = res.size() - 1; i >= 0; i--) {
    std::cout << "[";
    if (res[i].status == -1) std::cout << "refunded";
    else if (res[i].status == 0) std::cout << "pending";
    else if (res[i].status == 1) std::cout << "success";
    std::cout << "] ";
    std::cout << res[i].trainID << " " << res[i].st_sta << " " << res[i].leave_time << " -> " << res[i].ed_sta << " " << res[i].arriv_time << " " << res[i].price << " " << res[i].ticketNum << std::endl;
  }

  return 0;
}

int TicketSystem::refund_ticket(const username& UserName, const int& pos) {
  if (!UserSys.UserStat[UserName]) throw(exceptions("Not loginned"));

  auto res = UserTicketCnt.find(getHash(UserName));
  int userTicketCnt = 0;
  if (res) UserTicketCnt.readVal(res, userTicketCnt);
  if (userTicketCnt < pos) throw(exceptions("Order not found"));

  res = UserTicketData.find(std::make_pair(getHash(UserName), userTicketCnt - pos + 1));
  user_ticket userTicket; UserTicketData.readVal(res, userTicket);

  if (userTicket.status == -1) throw(exceptions("Order already refunded"));
  int status = userTicket.status;
  userTicket.status = -1;
  UserTicketData.writeVal(res, userTicket);

  if (status) {
    int ticket_pos = TrainSys.TicketData.find(std::make_pair(getHash(userTicket.trainID), userTicket.days));
    train_ticket ticket; TrainSys.TicketData.readVal(ticket_pos, ticket);
    ticket.modify_ticket(userTicket.st, userTicket.ed, userTicket.ticketNum);

    auto iter_st = PendData.lower_bound(std::make_pair(std::make_pair(getHash(userTicket.trainID), userTicket.days), 0));
    auto iter_ed = PendData.lower_bound(std::make_pair(std::make_pair(getHash(userTicket.trainID), userTicket.days), INT32_MAX));

    for (; iter_st != iter_ed; ++iter_st) {
      pend_ticket pend = iter_st.get_val();
      if (pend.ticketNum <= ticket.query_ticket(pend.st, pend.ed)) {
        UserTicketData.readVal(pend.user_ticket_pos, userTicket);
        PendData.del(std::make_pair(std::make_pair(getHash(userTicket.trainID), userTicket.days), iter_st.get_key().second));
        if (userTicket.status == -1) continue;
        userTicket.status = 1;
        UserTicketData.writeVal(pend.user_ticket_pos, userTicket);
        ticket.modify_ticket(pend.st, pend.ed, -pend.ticketNum);
      }
    }

    TrainSys.TicketData.writeVal(ticket_pos, ticket);
  }

  return 0;
}

int TicketSystem::clear() {
  UserTicketCnt.clear();
  UserTicketData.clear();
  PendData.clear();
  UserSys.UserData.clear();
  UserSys.UserStat.clear();
  TrainSys.PassData.clear();
  TrainSys.TicketData.clear();
  TrainSys.TrainData.clear();
  return 0;
}