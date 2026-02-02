#include <sstream>

#include "transport.hh"
#include "testing.hh"

static void send_recv()
{
  std::string content = "hello";
  std::stringstream stream;
  std::string received;

  EXPECT_NO_THROW(transport::send(content, stream));
  EXPECT_NO_THROW(received = transport::recv(stream));
  EXPECT_EQUAL(content.c_str(), received.c_str());
}

SET_TEST_NAMESPACE(transport);
REGISTER_TEST(send_recv);
