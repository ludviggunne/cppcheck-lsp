#ifndef TESTING_HH_INCLUDED
#define TESTING_HH_INCLUDED

#include <functional>
#include <string>
#include <vector>
#include <exception>

namespace testing {

  class Exception;

  class Test {

    friend class Suite;

  public:
    Test(const std::string &name, void (*function)(void));

  private:
    std::string name;
    void (*function)(void);
  };

  class Suite {

    friend class Test;

  private:
    Suite()
    {
    }

    ~Suite()
    {
    }

  private:
    void register_test(const Test *test);

  private:
    std::vector<const Test *> m_tests;
    std::vector<std::pair<std::string, Exception>> m_failures;

  public:
    void run();

  public:
    static Suite &instance();
  };

  class Exception : std::exception {
  public:
    Exception(const std::string &message, int line, const std::string &file)
      : message(message)
      , line(line)
      , file(file)
    {
    }

  public:
    std::string message;
    int line;
    std::string file;
  };

  void expect(bool condition, const std::string &message, int line, const std::string &file);
  void expect_equal(int lhs, int rhs, int line, const std::string &file);
  void expect_equal(const char *lhs, const char *rhs, int line, const std::string &file);
  void expect_throw(std::function<void(void)> function, const std::string &message, int line, const std::string &file);
  void expect_no_throw(std::function<void(void)> function, int line, const std::string &file);
}

#define SET_TEST_NAMESPACE(namespace) static const std::string test_namespace_ = #namespace
#define REGISTER_TEST(function) static testing::Test register_ ## function(test_namespace_ + ":" + #function, function)
#define EXPECT(condition, message) testing::expect(condition, message, __LINE__, __FILE__)
#define EXPECT_EQUAL(lhs, rhs) testing::expect_equal(lhs, rhs, __LINE__, __FILE__)
#define EXPECT_NO_THROW(statement) testing::expect_no_throw([&]() { statement; }, __LINE__, __FILE__)
#define EXPECT_THROW(statement, message) testing::expect_throw([&]() { statement; }, message, __LINE__, __FILE__)

#endif
