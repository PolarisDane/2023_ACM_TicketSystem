#include <iostream>
#include <cstring>
#include "UserSystem.h"
#include "date.h"

signed main(void) {
  Time t(4, 26, 17, 32);
  std::cout << t << std::endl;
  std::cout << t - 59 << std::endl;
  //UserSystem user;
  //std::string curUser = "yyu";
  //std::string UserName = "Polaris_Dane";
  //std::string Password = "5y57576";
  //try {
  //  user.addUser("yyu", "Polaris_Dane", "5y57576", "陈一星", "2488721971@qq.com", 1);
  //  user.loginUser("Polaris_Dane", "5y57576");
  //  //user.logoutUser("Polaris_Dane");
  //  //user.loginUser("Polaris_Dane", "y57576");
  //  user.queryProfile("Polaris_Dane", "Polaris_Dane");
  //  user.modifyProfile("Polaris_Dane", "Polaris_Dane", "");
  //}
  //catch (exceptions& e) {
  //  std::cout << e.error() << std::endl;
  //}
  return 0;
}