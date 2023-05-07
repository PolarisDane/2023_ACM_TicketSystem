#ifndef ACM_TICKETSYSTEM_USERSYSTEM
#define ACM_TICKETSYSTEM_USERSYSTEM

#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <cmath>
#include "MyString.h"
#include "hashmap.h"
#include "vector.h"
#include "BPT.h"

class User {
public:
  username UserName;
  userpassword UserPassword;
  userrealname UserRealName;
  usermail UserMail;
  int privilege;
  User() = default;
  User(const username& _UserName, const userpassword& _UserPassword, const userrealname& _UserRealName, const usermail& _UserMail, const int& p) :
    UserName(_UserName), UserPassword(_UserPassword), UserRealName(_UserRealName), UserMail(_UserMail), privilege(p) {}
  ~User() = default;
};

class UserSystem {

  friend class TicketSystem;

public:
  
  UserSystem() :UserData("User") {}
  ~UserSystem() = default;

  int addUser(const username& curUser, const username& newName, const userpassword& newPassword,
    const userrealname& newRealname, const usermail& newMail, const int& newP);
  int loginUser(const username& UserName, const userpassword& UserPassword);
  int logoutUser(const username& UserName);

  int queryProfile(const username& curUser, const username& UserName);
  int modifyProfile(const username& curUser, const username& UserName, const userpassword& newPassword,
    const userrealname& newRealname, const usermail& newMail, const int& newP);

private:

  BPTree<size_t, User> UserData;
  HashMap<username, bool, Stringhash>UserStat;

};

#endif