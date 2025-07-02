#include "testing.hh"
#include "logging.hh"

int main(int argc, char **argv)
{
  log_::enable(log_::Level::None);
  testing::Suite::instance().run();
}
