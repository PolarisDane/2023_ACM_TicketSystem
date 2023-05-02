#include "UserSystem.h"

int UserSystem::addUser(const username& curUser, const username& newName, const userpassword& newPassword, 
  const userrealname& newRealname, const usermail& newMail, const int& newP) {
  if (!UserData.dataCnt) {
    User newUser(newName, newPassword, newRealname, newMail, 10);
    UserData.data.insert(newName, UserData.dataCnt);
    UserData.write(UserData.dataCnt++, newUser);
    UserStat[newName] = 0;
    return 0;
  }

  if (!UserStat[curUser]) throw(exceptions("Not loginned"));

  auto res = UserData.data.find(curUser);
  if (res.empty()) throw(exceptions("User doesn't exist"));
  User user; UserData.read(res[0], user);

  if (UserData.data.count(newName)) throw(exceptions("Identical username"));
  if (user.privilege < newP) throw(exceptions("Authority not enough"));

  User newUser(newName, newPassword, newRealname, newMail, newP);
  UserData.data.insert(newName, UserData.dataCnt);
  UserData.write(UserData.dataCnt++, newUser);
  UserStat[newName] = 0;
  return 0;
}

int UserSystem::loginUser(const username& UserName, const userpassword& UserPassword) {
  if (!UserData.data.count(UserName)) throw(exceptions("User doesn't exist"));
  if (UserStat[UserName]) throw(exceptions("Already loginned"));

  auto res = UserData.data.find(UserName);
  if (res.empty()) throw(exceptions("User doesn't exist"));
  User user; UserData.read(res[0], user);

  if (user.UserPassword != UserPassword) throw(exceptions("Wrong password"));

  UserStat[UserName] = true;
  return 0;
}

int UserSystem::logoutUser(const username& UserName) {
  if (!UserStat.find(UserName)) throw(exceptions("Not loginned"));
  UserStat[UserName] = false;
  return 0;
}

int UserSystem::queryProfile(const username& curUser, const username& UserName) {
  if (!UserStat[curUser]) throw(exceptions("Not loginned"));
  if (!UserData.data.count(UserName)) throw(exceptions("User doesn't exist"));

  auto res = UserData.data.find(curUser);
  if (res.empty()) throw(exceptions("User doesn't exist"));
  User user; UserData.read(res[0], user);

  res = UserData.data.find(UserName);
  if (res.empty()) throw(exceptions("Target user doesn't exist"));
  User tar; UserData.read(res[0], tar);

  if (user.privilege <= tar.privilege && curUser != UserName) throw(exceptions("Authority not enough"));

  std::cout << tar.UserName << " " << tar.UserRealName << " " << tar.UserMail << " " << tar.privilege << std::endl;
  return 0;
}

int UserSystem::modifyProfile(const username& curUser, const username& UserName, const userpassword& newPassword,
  const userrealname& newRealname, const usermail& newMail, const int& newP) {
  if (!UserStat[curUser]) throw(exceptions("Not loginned"));

  auto res = UserData.data.find(curUser);
  if (res.empty()) throw(exceptions("User doesn't exist"));
  User user; UserData.read(res[0], user);

  res = UserData.data.find(UserName);
  if (res.empty()) throw(exceptions("Target user doesn't exist"));
  User tar; UserData.read(res[0], tar);

  if (user.privilege < tar.privilege && curUser != UserName) throw(exceptions("Authority not enough"));

  if (!newPassword.empty()) tar.UserPassword = newPassword;
  if (!newRealname.empty()) tar.UserRealName = newRealname;
  if (!newMail.empty()) tar.UserMail = newMail;
  if (newP != -1) tar.privilege = newP;

  UserData.write(res[0], tar);
  std::cout << tar.UserName << " " << tar.UserRealName << " " << tar.UserMail << " " << tar.privilege << std::endl;
  return 0;
}

