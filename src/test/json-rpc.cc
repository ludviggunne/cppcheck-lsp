#include "testing.hh"
#include "json-rpc.hh"

static void add_version()
{
  picojson::object message;
  std::string result;

  EXPECT_NO_THROW(message["key"] = picojson::value("value"));
  EXPECT_NO_THROW(json_rpc::add_version(message));
  EXPECT_NO_THROW(result = picojson::value(std::move(message)).serialize());
  EXPECT_EQUAL(result.c_str(), "{\"jsonrpc\":\"2.0\",\"key\":\"value\"}");
}

static void parse()
{
  std::string raw = "{\"key\":\"value\"}";
  EXPECT_THROW(json_rpc::parse(raw), "Missing JSON-RPC version");

  raw = "{\"jsonrpc\":\"1.0\",\"key\":\"value\"}";
  EXPECT_THROW(json_rpc::parse(raw), "Invalid JSON-RPC version 1.0");

  raw = "{\"jsonrpc\":\"2.0\",\"key\":\"value\"}";
  EXPECT_NO_THROW(json_rpc::parse(raw));
}

SET_TEST_NAMESPACE(json_rpc);
REGISTER_TEST(add_version);
REGISTER_TEST(parse);
