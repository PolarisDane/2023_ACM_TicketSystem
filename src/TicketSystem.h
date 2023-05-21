#ifndef ACM_TICKETSYSTEM
#define ACM_TICKETSYSTEM

#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <cmath>
#include "UserSystem.h"
#include "TrainSystem.h"

class user_ticket {
public:
  username user;
  int status;
  //1:success
  //0:pending
  //-1:refunded
  trainid trainID;
  Date st_date;
  station st_sta, ed_sta;
  int ticketNum;
  Time arriv_time, leave_time;
  int price;
  int days;
  int st, ed;

public:
  user_ticket() = default;
  ~user_ticket() = default;

  user_ticket(const username& _user, const int& _status, const trainid& _trainID, const Date& _st_date, const station& _st_sta, const station& _ed_sta, const int& _ticketNum, 
    const Time& _arriv_time, const Time& _leave_time, const int& _price, const int& _days, const int& _st, const int& _ed)
    :user(_user), status(_status), trainID(_trainID), st_date(_st_date), st_sta(_st_sta), ed_sta(_ed_sta), ticketNum(_ticketNum), arriv_time(_arriv_time), leave_time(_leave_time), price(_price), days(_days), st(_st), ed(_ed) {}
};

class pend_ticket {
public:
  username user;
  trainid trainID;
  int ticketNum;
  int st, ed;
  int user_ticket_pos;//对应用户票的外存位置
  
public:
  pend_ticket() = default;
  ~pend_ticket() = default;

  pend_ticket(const username& _user, const trainid& _trainID, const int& _st, const int& _ed, const int& _ticketNum, const int& _user_ticket_pos)
    :user(_user), trainID(_trainID), st(_st), ed(_ed), ticketNum(_ticketNum), user_ticket_pos(_user_ticket_pos) {}
};

class TicketSystem {
public:
  UserSystem UserSys;
  TrainSystem TrainSys;
  BPTree<size_t, int> UserTicketCnt;
  BPTree<std::pair<size_t,int>, user_ticket> UserTicketData;
  BPTree<std::pair<std::pair<size_t, int>, int>, pend_ticket> PendData;
public:
  TicketSystem() :UserTicketCnt("UserTicketCnt"), UserTicketData("UserTicket"), PendData("Pend") {}
  ~TicketSystem() = default;

  int buy_ticket(const username& UserName, const trainid& trainID, const Date& date, const int& ticketNum,
    const station& st_sta, const station& ed_sta, const bool& type);
  int query_order(const username& UserName);
  int refund_ticket(const username& UserName, const int& pos);
  int clear();
  void exit();
};


#endif