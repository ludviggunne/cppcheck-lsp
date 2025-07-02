#include <iostream>

#include "testing.hh"

testing::Test::Test(const std::string &name, void (*function)(void))
  : name(name)
  , function(function)
{
  Suite::instance().register_test(this);
}

testing::Suite &testing::Suite::instance()
{
  static Suite suite;
  return suite;
}

void testing::Suite::register_test(const testing::Test *test)
{
  m_tests.push_back(test);
}

void testing::Suite::run()
{
  int total = 0;
  int succeeded = 0;
  int failed = 0;

  for (const Test *const test : m_tests) {
    std::cout << test->name << std::endl;
    try {
      test->function();
      succeeded++;
    } catch (const testing::Exception &exception) {
      m_failures.push_back({test->name, exception});
      failed++;
    }
    total++;
  }

  std::cout << "\n\n";
  std::cout << "Total: " << total << std::endl;
  std::cout << "Succeeded: " << succeeded << std::endl;
  std::cout << "Failed: " << failed << std::endl;

  for (const auto &[name, failure] : m_failures) {
    std::cerr << "\n\n";
    std::cerr << name << " (" << failure.file << ":" << failure.line << "):" << std::endl;
    std::cerr << failure.message << std::endl;
  }
}

void testing::expect(bool condition, const std::string &message, int line, const std::string &file)
{
  if (!condition)
    throw Exception(message, line, file);
}

void testing::expect_equal(const char *lhs, const char *rhs, int line, const std::string &file)
{
  if (std::string(lhs) != std::string(rhs)) {
    const std::string message =
      std::string("lhs != rhs:\n") +
      "\n"
      "lhs:\n" +
      lhs + "\n"
      "\n" +
      "rhs:\n" +
      rhs + "\n";
    expect(false, message, line, file);
  }
}

void testing::expect_equal(const int lhs, const int rhs, int line, const std::string &file)
{
  if (lhs != rhs) {
    const std::string message = std::to_string(lhs) + " != " + std::to_string(rhs);
    expect(false, message, line, file);
  }
}

void testing::expect_no_throw(std::function<void(void)> function, int line, const std::string &file)
{
  try {
    function();
  } catch (const std::exception &exception) {
    const std::string message = std::string("Exception thrown: ") + exception.what();
    throw Exception(message, line, file);
  }
}

void testing::expect_throw(std::function<void(void)> function, const std::string &message, int line, const std::string &file)
{
  try {
    function();
  } catch (const std::exception &exception) {
    if (exception.what() != message) {
      std::string new_message;
      new_message += "Exception message mismatch:\n";
      new_message += "  Expected: " + message + "\n";
      new_message += "  Actual: " + std::string(exception.what()) + "\n";
      expect(false, new_message, line, file);
    }
    return;
  }
  const std::string new_message = "Expected exception: " + message;
  throw Exception(new_message, line, file);
}
