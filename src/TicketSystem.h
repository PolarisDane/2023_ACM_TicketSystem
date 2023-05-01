#ifndef ACM_TICKETSYSTEM
#define ACM_TICKETSYSTEM

#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <cmath>
#include "UserSystem.h"
#include "TrainSystem.h"

class TicketSystem {
public:
  UserSystem UserSys;
  TrainSystem TrainSys;
public:
  TicketSystem() = default;
  ~TicketSystem() = default;

  //int buy_ticket(const User& curUser, const trainid& trainID, const Date& date, const int& ticket_num,
  //  const station& st_sta, const station& ed_sta, const bool& type);
  //int query_order(const User& curUser);
  //int refund_ticket(const User& curUser, const int& ticket);
};


#endif