#include "TicketSystem.h"

int TicketSystem::buy_ticket(const User& curUser, const trainid& trainID, const Date& date, const int& ticket_num,
  const station& st_sta, const station& ed_sta, const bool& type) {
  if (!UserSys.UserStat[curUser]) throw(exceptions("Not loginned"));

  auto res = TrainSys.TrainData.data.find(trainID);
  if (!res[0]) throw(exceptions("Train doesn't exist"));
  Train train; TrainSys.TrainData.read(res[0], train);

  int st = 0, ed = 0;
  for (int i = 1; i <= train.stationNum; i++) {
    if (train.stations[i] == st_sta) st = i;
    if (train.stations[i] == ed_sta) ed = i;
  }
  if (!st || !ed || st >= ed) throw(exceptions("Ticket not found"));

  int days = (train.st_time + train.leave_time[st]).date - date;
  res = TrainSys.TicketData.data.find(std::make_pair(trainID, days));
  train_ticket ticket; TrainSys.TicketData.read(res[0], ticket);

  if (ticket.query_ticket(st, ed) >= ticket_num) {

    UserTicketData
  }
  else {
    if (type) {

    }
  }

  return 0;
}

int TicketSystem::query_order(const User& curUser) {

}

int TicketSystem::refund_ticket(const User& curUser, const int& ticket) {

}