#include <iostream>
#include <cstring>
#include "MyString.h"
#include "date.h"
#include "exceptions.h"
#include "Parser.h"
#include "TicketSystem.h"

signed main(void) {
  TicketSystem sys;
  std::string opt;
  std::string command;
  std::string buf[30];
  int cnt;
  while (1) {
    std::getline(std::cin, command);
    ReadLine(command, buf);
    opt = buf[2];
    cnt = std::stoi(buf[0]);
    std::cout << buf[1] << " ";
    try {
      if (opt == "add_user") {
        username curUser, newName;
        userpassword newPassword;
        userrealname newRealname;
        usermail newMail;
        int newP;
        for (int i = 3; i <= cnt; i += 2) {
          if (buf[i] == "-c") curUser = buf[i + 1];
          else if (buf[i] == "-u") newName = buf[i + 1];
          else if (buf[i] == "-p") newPassword = buf[i + 1];
          else if (buf[i] == "-n") newRealname = buf[i + 1];
          else if (buf[i] == "-m") newMail = buf[i + 1];
          else if (buf[i] == "-g") newP = std::stoi(buf[i + 1]);
          else throw(exceptions("Invalid argument"));
        }
        sys.UserSys.addUser(curUser, newName, newPassword, newRealname, newMail, newP);
        std::cout << "0" << std::endl;
      }
      else if (opt == "login") {
        username UserName;
        userpassword UserPassword;
        for (int i = 3; i <= cnt; i += 2) {
          if (buf[i] == "-u") UserName = buf[i + 1];
          else if (buf[i] == "-p") UserPassword = buf[i + 1];
          else throw(exceptions("Invalid argument"));
        }
        sys.UserSys.loginUser(UserName, UserPassword);
        std::cout << "0" << std::endl;
      }
      else if (opt == "logout") {
        username UserName;
        for (int i = 3; i <= cnt; i += 2) {
          if (buf[i] == "-u") UserName = buf[i + 1];
          else throw(exceptions("Invalid argument"));
        }
        sys.UserSys.logoutUser(UserName);
        std::cout << "0" << std::endl;
      }
      else if (opt == "query_profile") {
        username curUser, UserName;
        for (int i = 3; i <= cnt; i += 2) {
          if (buf[i] == "-c") curUser = buf[i + 1];
          else if (buf[i] == "-u") UserName = buf[i + 1];
          else throw(exceptions("Invalid argument"));
        }
        sys.UserSys.queryProfile(curUser, UserName);
      }
      else if (opt == "modify_profile") {
        username curUser, UserName;
        userpassword newPassword;
        userrealname newRealname;
        usermail newMail;
        int newP = -1;
        for (int i = 3; i <= cnt; i += 2) {
          if (buf[i] == "-c") curUser = buf[i + 1];
          else if (buf[i] == "-u") UserName = buf[i + 1];
          else if (buf[i] == "-p") newPassword = buf[i + 1];
          else if (buf[i] == "-n") newRealname = buf[i + 1];
          else if (buf[i] == "-m") newMail = buf[i + 1];
          else if (buf[i] == "-g") newP = std::stoi(buf[i + 1]);
          else throw(exceptions("Invalid argument"));
        }
        sys.UserSys.modifyProfile(curUser, UserName, newPassword, newRealname, newMail, newP);
      }
      else if (opt == "add_train") {
        trainid newID;
        station stations[102];
        int newstationNum, newseatNum, price[102], new_trav_time[102], new_stop_time[102];
        Time new_st_time;
        Date new_st_Date, new_ed_Date;
        char newType;
        for (int i = 3; i <= cnt; i += 2) {
          if (buf[i] == "-i") newID = buf[i + 1];
          else if (buf[i] == "-n") newstationNum = std::stoi(buf[i + 1]);
          else if (buf[i] == "-m") newseatNum = std::stoi(buf[i + 1]);
          else if (buf[i] == "-s") parse_station(stations, buf[i + 1]);
          else if (buf[i] == "-p") parse_num(price, buf[i + 1]);
          else if (buf[i] == "-x") parse_time_point(new_st_time, buf[i + 1]);
          else if (buf[i] == "-t") parse_num(new_trav_time, buf[i + 1]);
          else if (buf[i] == "-o") parse_num(new_stop_time, buf[i + 1]);
          else if (buf[i] == "-d") parse_st_ed_date(new_st_Date, new_ed_Date, buf[i + 1]);
          else if (buf[i] == "-y") newType = buf[i + 1][0];
          else throw(exceptions("Invalid argument"));
        }
        sys.TrainSys.add_train(newID, newstationNum, newseatNum, stations, price, new_st_time, new_trav_time, new_stop_time, new_st_Date, new_ed_Date, newType);
        std::cout << "0" << std::endl;
      }
      else if (opt == "delete_train") {
        trainid trainID;
        for (int i = 3; i <= cnt; i += 2) {
          if (buf[i] == "-i") trainID = buf[i + 1];
          else throw(exceptions("Invalid argument"));
        }
        sys.TrainSys.delete_train(trainID);
        std::cout << "0" << std::endl;
      }
      else if (opt == "release_train") {
        trainid trainID;
        for (int i = 3; i <= cnt; i += 2) {
          if (buf[i] == "-i") trainID = buf[i + 1];
          else throw(exceptions("Invalid argument"));
        }
        sys.TrainSys.release_train(trainID);
        std::cout << "0" << std::endl;
      }
      else if (opt == "query_train") {
        trainid trainID;
        Date date;
        for (int i = 3; i <= cnt; i += 2) {
          if (buf[i] == "-i") trainID = buf[i + 1];
          else if (buf[i] == "-d") parse_date(date, buf[i + 1]);
          else throw(exceptions("Invalid argument"));
        }
        sys.TrainSys.query_train(trainID, date);
      }
      else if (opt == "query_ticket") {
        station st_sta, ed_sta;
        Date date;
        int opt;
        for (int i = 3; i <= cnt; i += 2) {
          if (buf[i] == "-s") st_sta = buf[i + 1];
          else if (buf[i] == "-t") ed_sta = buf[i + 1];
          else if (buf[i] == "-d") parse_date(date, buf[i + 1]);
          else if (buf[i] == "-p") {
            if (buf[i + 1] == "time") opt = 0;
            else if (buf[i + 1] == "cost") opt = 1;
          }
          else throw(exceptions("Invalid argument"));
        }
        sys.TrainSys.query_ticket(st_sta, ed_sta, date, opt);
      }
      else if (opt == "query_transfer") {
        station st_sta, ed_sta;
        Date date;
        int opt;
        for (int i = 3; i <= cnt; i += 2) {
          if (buf[i] == "-s") st_sta = buf[i + 1];
          else if (buf[i] == "-t") ed_sta = buf[i + 1];
          else if (buf[i] == "-d") parse_date(date, buf[i + 1]);
          else if (buf[i] == "-p") {
            if (buf[i + 1] == "time") opt = 0;
            else if (buf[i + 1] == "cost") opt = 1;
          }
          else throw(exceptions("Invalid argument"));
        }
        sys.TrainSys.query_transfer(st_sta, ed_sta, date, opt);
      }
      else if (opt == "buy_ticket") {

      }
      else if (opt == "query_order") {

      }
      else if (opt == "refund_ticket") {

      }
      else if (opt == "clean") {
        //Not finished
      }
      else if (opt == "exit") {
        std::cout << "bye" << std::endl;
        break;
      }
      else throw(exceptions("Undefined"));
    }
    catch (exceptions& e) {
      std::cout << "-1" << std::endl;
      std::cout << e.error() << std::endl;
    }
  }
  return 0;
}