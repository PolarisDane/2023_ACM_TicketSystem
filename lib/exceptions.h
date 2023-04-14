#ifndef ACM_TICKETSYSTEM_EXCEPTIONS
#define ACM_TICKETSYSTEM_EXCEPTIONS

#include <cstring>
#include <string>

class exceptions {
private:
  std::string what;
public:
  exceptions() {}
  exceptions(const std::string& str) {
    what = str;
  }
  std::string error() { return what; }
};

#endif