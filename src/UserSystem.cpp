#include "UserSystem.h"

UserSystem::UserSystem() :UserData("User") {
  if (!UserData.size()) {
    user_cnt = 0;
    std::fstream create;
    create.open("User.data", std::ios::out); create.close();
    user_file.open("User.data", std::ios::binary | std::ios::out | std::ios::in);
  }
  else {
    user_file.open("User.data", std::ios::binary | std::ios::out | std::ios::in);
    user_file.seekg(0);
    user_file.read(reinterpret_cast<char*>(&user_cnt), sizeof(int));
  }
}

UserSystem::~UserSystem() {
  user_file.seekp(0);
  user_file.write(reinterpret_cast<char*>(&user_cnt), sizeof(int));
  user_file.close();
}

void UserSystem::readUser(const int& pos, User& p) {
  user_file.seekg(pos * sizeof(User) + sizeof(int));
  user_file.read(reinterpret_cast<char*>(&p), sizeof(User));
}
void UserSystem::writeUser(const int& pos, User& p) {
  user_file.seekp(pos * sizeof(User) + sizeof(int));
  user_file.write(reinterpret_cast<char*>(&p), sizeof(User));
}

int UserSystem::addUser(const username& curUser, const username& newName, const userpassword& newPassword, 
  const userrealname& newRealname, const usermail& newMail, const int& newP) {
  if (!user_cnt) {
    User newUser(newName, newPassword, newRealname, newMail, 10);
    UserData.insert(newName, user_cnt);
    writeUser(user_cnt++, newUser);
    return 0;
  }

  if (!UserStat[curUser]) throw(exceptions("Not loginned"));

  auto res = UserData.find(curUser);
  if (res.empty()) throw(exceptions("User doesn't exist"));
  int curPos = res[0];
  User user; readUser(curPos, user);

  if (UserData.count(newName)) throw(exceptions("Identical username"));
  if (user.privilege < newP) throw(exceptions("Authority not enough"));

  User newUser(newName, newPassword, newRealname, newMail, newP);
  UserData.insert(newName, user_cnt);
  writeUser(user_cnt++, newUser);
  return 0;
}

int UserSystem::loginUser(const username& UserName, const userpassword& UserPassword) {
  if (!UserData.count(UserName)) throw(exceptions("User doesn't exist"));
  if (UserStat[UserName]) throw(exceptions("Already loginned"));

  auto res = UserData.find(UserName);
  if (res.empty()) throw(exceptions("User doesn't exist"));
  int curPos = res[0];
  User user; readUser(curPos, user);

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
  if (!UserData.count(UserName)) throw(exceptions("User doesn't exist"));

  auto res = UserData.find(curUser);
  if (res.empty()) throw(exceptions("User doesn't exist"));
  int curPos = res[0];
  User user; readUser(curPos, user);

  res = UserData.find(UserName);
  if (res.empty()) throw(exceptions("Target user doesn't exist"));
  int tarPos = res[0];
  User tar; readUser(tarPos, tar);

  if (user.privilege<tar.privilege) throw(exceptions("Authority not enough"));

  std::cout << tar.UserName << " " << tar.UserRealName << " " << tar.UserMail << " " << tar.privilege << std::endl;
  return 0;
}

int UserSystem::modifyProfile(const username& curUser, const username& UserName, const userpassword& newPassword,
  const userrealname& newRealname, const usermail& newMail, const int& newP) {
  if (!UserStat[curUser]) throw(exceptions("Not loginned"));

  auto res = UserData.find(curUser);
  if (res.empty()) throw(exceptions("User doesn't exist"));
  int curPos = res[0];
  User user; readUser(curPos, user);

  res = UserData.find(UserName);
  if (res.empty()) throw(exceptions("Target user doesn't exist"));
  int tarPos = res[0];
  User tar; readUser(tarPos, tar);

  if (user.privilege < tar.privilege) throw(exceptions("Authority not enough"));

  tar.UserPassword = newPassword;
  tar.UserRealName = newRealname;
  tar.UserMail = newMail;
  tar.privilege = newP;
  //fixing

  writeUser(tarPos, tar);
  return 0;
}

