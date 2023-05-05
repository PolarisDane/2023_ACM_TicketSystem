#include "TicketSystem.h"

int TicketSystem::buy_ticket(const username& UserName, const trainid& trainID, const Date& date, const int& ticketNum,
  const station& st_sta, const station& ed_sta, const bool& type) {
  if (!UserSys.UserStat[UserName]) throw(exceptions("Not loginned"));

  auto res = TrainSys.TrainData.data.find(trainID);
  if (res.empty()) throw(exceptions("Train doesn't exist"));
  Train train; TrainSys.TrainData.read(res[0], train);

  if (!train.release) throw(exceptions("Train not released"));

  if (ticketNum > train.seatNum) throw(exceptions("Expected tickets exceeds total tickets"));
  //超过总数则不用再进行任何处理

  int st = 0, ed = 0;
  for (int i = 1; i <= train.stationNum; i++) {
    if (train.stations[i] == st_sta) st = i;
    if (train.stations[i] == ed_sta) ed = i;
  }
  if (!st || !ed || st >= ed) throw(exceptions("Ticket not found"));

  int days = date - (train.st_time + train.leave_time[st]).date;
  if (days > train.ed_date - train.st_date || days < 0) throw(exceptions("Train doesn't exist"));

  res = TrainSys.TicketData.data.find(std::make_pair(trainID, days));
  train_ticket ticket; TrainSys.TicketData.read(res[0], ticket);

  user_ticket userTicket(UserName, 1, trainID, train.st_date + days, st_sta, ed_sta,
    ticketNum, train.st_time + days * 1440 + train.arriv_time[ed], train.st_time + days * 1440 + train.leave_time[st], train.price[ed] - train.price[st], days, st, ed);

  if (ticket.query_ticket(st, ed) >= ticketNum) {
    UserTicketData.data.insert(UserName, UserTicketData.dataCnt);
    UserTicketData.write(UserTicketData.dataCnt++, userTicket);

    ticket.modify_ticket(st, ed, -ticketNum);
    TrainSys.TicketData.write(res[0], ticket);

    std::cout << (long long)ticketNum * (train.price[ed] - train.price[st]) << std::endl;
    return 0;
  }
  else {
    if (type) {
      pend_ticket pend(UserName, trainID, st, ed, ticketNum, UserTicketData.dataCnt);
      PendData.data.insert(std::make_pair(trainID, days), PendData.dataCnt);
      PendData.write(PendData.dataCnt++, pend);

      userTicket.status = 0;
      UserTicketData.data.insert(UserName, UserTicketData.dataCnt);
      UserTicketData.write(UserTicketData.dataCnt++, userTicket);

      std::cout << "queue" << std::endl;
    }
    else throw(exceptions("Ticket not enough"));
  }

  return 0;
}

int TicketSystem::query_order(const username& UserName) {
  if (!UserSys.UserStat[UserName]) throw(exceptions("Not loginned"));

  auto res = UserTicketData.data.find(UserName);
  std::cout << res.size() << std::endl;
  for (int i = res.size() - 1; i >= 0; i--) {
    user_ticket userTicket; UserTicketData.read(res[i], userTicket);
    std::cout << "[";
    if (userTicket.status == -1) std::cout << "refunded";
    else if (userTicket.status == 0) std::cout << "pending";
    else if (userTicket.status == 1) std::cout << "success";
    std::cout << "] ";
    std::cout << userTicket.trainID << " " << userTicket.st_sta << " " << userTicket.leave_time << " -> " << userTicket.ed_sta << " " << userTicket.arriv_time << " " << userTicket.price << " " << userTicket.ticketNum << std::endl;
  }

  return 0;
}

int TicketSystem::refund_ticket(const username& UserName, const int& pos) {
  if (!UserSys.UserStat[UserName]) throw(exceptions("Not loginned"));

  auto res = UserTicketData.data.find(UserName);
  if (res.size() < pos) throw(exceptions("Order not found"));

  user_ticket userTicket; UserTicketData.read(res[res.size() - pos], userTicket);

  if (userTicket.status == -1) throw(exceptions("Order already refunded"));
  int status = userTicket.status;
  userTicket.status = -1;
  UserTicketData.write(res[res.size() - pos], userTicket);

  if (status) {
    res = TrainSys.TicketData.data.find(std::make_pair(userTicket.trainID, userTicket.days));
    int ticketPos = res[0];
    train_ticket ticket; TrainSys.TicketData.read(ticketPos, ticket);
    ticket.modify_ticket(userTicket.st, userTicket.ed, userTicket.ticketNum);

    res = PendData.data.find(std::make_pair(userTicket.trainID, userTicket.days));
    for (int i = 0; i < res.size(); i++) {
      pend_ticket pend; PendData.read(res[i], pend);
      if (pend.ticketNum <= ticket.query_ticket(pend.st, pend.ed)) {
        UserTicketData.read(pend.user_ticket_pos, userTicket);
        PendData.data.del(std::make_pair(userTicket.trainID, userTicket.days), res[i]);
        if (userTicket.status == -1) continue;//可能已经被退款了
        userTicket.status = 1;
        UserTicketData.write(pend.user_ticket_pos, userTicket);
        ticket.modify_ticket(pend.st, pend.ed, -pend.ticketNum);
      }
    }

    TrainSys.TicketData.write(ticketPos, ticket);
  }

  return 0;
}