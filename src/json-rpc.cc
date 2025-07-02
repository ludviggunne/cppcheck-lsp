#include "json-rpc.hh"
#include "lsp/error.hh"

void json_rpc::add_version(picojson::object &message)
{
  message["jsonrpc"] = picojson::value("2.0");
}

picojson::object json_rpc::parse(const std::string &raw)
{
  picojson::value value;
  std::string error;
  picojson::object message;

  error = picojson::parse(value, raw);
  if (!error.empty()) {
    throw lsp::Exception(lsp::ErrorCode::ParseError, error.c_str());
  }

  try {
    message = value.get<picojson::object>();
  } catch (...) {
    throw lsp::Exception(lsp::ErrorCode::InvalidRequest, "Not an object");
  }

  if (message.count("jsonrpc") == 0) {
    throw lsp::Exception(lsp::ErrorCode::InvalidRequest, "Missing JSON-RPC version");
  }
  const std::string version = message.at("jsonrpc").get<std::string>();
  if (version != "2.0") {
    throw lsp::Exception(lsp::ErrorCode::InvalidRequest, "Invalid JSON-RPC version " + version);
  }

  return message;
}

picojson::value json_rpc::get_or_null(const picojson::object &object, const std::string &name)
{
  if (object.count(name) == 0)
    return picojson::value();
  return object.at(name);
}

const picojson::value &json_rpc::expect_field(const picojson::object &object, const std::string &name, lsp::ErrorCode errorcode)
{
  if (object.count(name) == 0)
    throw lsp::Exception(errorcode, "Missing field " + name);
  return object.at(name);
}
