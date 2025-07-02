#include "testing.hh"
#include "subprocess.hh"

static void run_echo()
{
  const char *argv[] = { "echo", "-e", "hello\ngoodbye", NULL };
  subprocess::Result result;

  EXPECT_NO_THROW(result = subprocess::run(argv));
  EXPECT_EQUAL(result.exitcode, 0);
  EXPECT_EQUAL(result.output.size(), 2);
  EXPECT_EQUAL(result.output[0].c_str(), "hello");
  EXPECT_EQUAL(result.output[1].c_str(), "goodbye");
}

SET_TEST_NAMESPACE(subprocess);
REGISTER_TEST(run_echo);
