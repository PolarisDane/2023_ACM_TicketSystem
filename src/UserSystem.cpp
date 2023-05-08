#include "UserSystem.h"

int UserSystem::addUser(const username& curUser, const username& newName, const userpassword& newPassword, 
  const userrealname& newRealname, const usermail& newMail, const int& newP) {
  if (UserData.empty()) {
    User newUser(newName, newPassword, newRealname, newMail, 10);
    UserData.insert(getHash(newName), newUser);
    return 0;
  }

  size_t newHash = getHash(newName), curHash = getHash(curUser);

  if (!UserStat[curUser]) throw(exceptions("Not loginned"));

  auto res = UserData.find(curHash);
  if (!res) throw(exceptions("User doesn't exist"));
  User user; UserData.readVal(res, user);

  if (UserData.count(newHash)) throw(exceptions("Identical username"));
  if (user.privilege <= newP) throw(exceptions("Authority not enough"));

  User newUser(newName, newPassword, newRealname, newMail, newP);
  UserData.insert(newHash, newUser);
  return 0;
}

int UserSystem::loginUser(const username& UserName, const userpassword& UserPassword) {
  if (!UserData.count(getHash(UserName))) throw(exceptions("User doesn't exist"));
  if (UserStat[UserName]) throw(exceptions("Already loginned"));

  auto res = UserData.find(getHash(UserName));
  if (!res) throw(exceptions("User doesn't exist"));
  User user; UserData.readVal(res, user);

  if (user.UserPassword != UserPassword) throw(exceptions("Wrong password"));

  UserStat[UserName] = true;
  return 0;
}

int UserSystem::logoutUser(const username& UserName) {
  if (!UserStat[UserName]) throw(exceptions("Not loginned"));
  UserStat[UserName] = false;
  return 0;
}

int UserSystem::queryProfile(const username& curUser, const username& UserName) {
  if (!UserStat[curUser]) throw(exceptions("Not loginned"));

  size_t tarHash = getHash(UserName), curHash = getHash(curUser);

  auto res = UserData.find(curHash);
  if (!res) throw(exceptions("User doesn't exist"));
  User user; UserData.readVal(res, user);

  res = UserData.find(tarHash);
  if (!res) throw(exceptions("Target user doesn't exist"));
  User tar; UserData.readVal(res, tar);

  if (user.privilege <= tar.privilege && curHash != tarHash) throw(exceptions("Authority not enough"));

  std::cout << tar.UserName << " " << tar.UserRealName << " " << tar.UserMail << " " << tar.privilege << std::endl;
  return 0;
}

int UserSystem::modifyProfile(const username& curUser, const username& UserName, const userpassword& newPassword,
  const userrealname& newRealname, const usermail& newMail, const int& newP) {
  if (!UserStat[curUser]) throw(exceptions("Not loginned"));

  size_t tarHash = getHash(UserName), curHash = getHash(curUser);

  auto res = UserData.find(curHash);
  if (!res) throw(exceptions("User doesn't exist"));
  User user; UserData.readVal(res, user);

  res = UserData.find(tarHash);
  if (!res) throw(exceptions("Target user doesn't exist"));
  User tar; UserData.readVal(res, tar);

  if (user.privilege <= tar.privilege && curHash != tarHash) throw(exceptions("Authority not enough"));
  if (user.privilege <= newP) throw(exceptions("Authority not enough"));

  if (!newPassword.empty()) tar.UserPassword = newPassword;
  if (!newRealname.empty()) tar.UserRealName = newRealname;
  if (!newMail.empty()) tar.UserMail = newMail;
  if (newP != -1) tar.privilege = newP;

  UserData.writeVal(res, tar);
  std::cout << tar.UserName << " " << tar.UserRealName << " " << tar.UserMail << " " << tar.privilege << std::endl;
  return 0;
}

