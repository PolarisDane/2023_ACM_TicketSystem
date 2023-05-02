#ifndef ACM_TICKETSYSTEM
#define ACM_TICKETSYSTEM

#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <cmath>
#include "UserSystem.h"
#include "TrainSystem.h"
#include "FileSystem.h"

class user_ticket {
public:

public:

};

class pend_ticket {
public:
  username user;

public:

};

class TicketSystem {
public:
  UserSystem UserSys;
  TrainSystem TrainSys;
  FileSystem<std::pair<username, int>, user_ticket> UserTicketData;
  FileSystem<std::pair<std::pair<trainid, int>, int>, pend_ticket> PendData;
public:
  TicketSystem() = default;
  ~TicketSystem() = default;

  int buy_ticket(const User& curUser, const trainid& trainID, const Date& date, const int& ticket_num,
    const station& st_sta, const station& ed_sta, const bool& type);
  int query_order(const User& curUser);
  int refund_ticket(const User& curUser, const int& ticket);
};


#endif