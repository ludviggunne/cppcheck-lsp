#include "testing.hh"
#include "cppcheck.hh"

static void error_message_parse()
{
  cppcheck::ErrorMessage message;
  std::string line = "some-file.c|12|13|warning|this is a message with a pipe character: | <- there it is";

  EXPECT(message.parse(line), "Parsing failed");
  EXPECT_EQUAL(message.file.c_str(), "some-file.c");
  EXPECT_EQUAL(message.line, 11);
  EXPECT_EQUAL(message.column, 12);
  EXPECT_EQUAL(message.severity.c_str(), "warning");
  EXPECT_EQUAL(static_cast<int>(message.lsp_severity), static_cast<int>(lsp::Severity::Warning));
  EXPECT_EQUAL(message.message.c_str(), "this is a message with a pipe character: | <- there it is");
}

static void command_argv()
{
  cppcheck::Config config;

  const char *expected_array[] = {
    "cppcheck",
    "-q",
    "--template='{file}|{line}|{column}|{severity}|{message}'",
    "file.c",
    nullptr,
  };

  const char **expected = expected_array;
  const char **actual = config.argv("file.c");

  for (; *actual && *expected; actual++, expected++) {
    EXPECT_EQUAL(*actual, *expected);
  }

  EXPECT(!(actual && !expected), "argv too long");
  EXPECT(!(!actual && expected), "argv too short");
}

SET_TEST_NAMESPACE(cppcheck);
REGISTER_TEST(error_message_parse);
REGISTER_TEST(command_argv);
