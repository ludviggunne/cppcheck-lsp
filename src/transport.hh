#ifndef TRANSPORT_HH_INCLUDED
#define TRANSPORT_HH_INCLUDED

#include <stdexcept>
#include <string>

namespace transport {
  void send(const std::string &content, std::ostream &os);
  std::string recv(std::istream &is);
};

#endif
