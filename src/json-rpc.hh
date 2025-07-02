#ifndef JSON_RPC_HH_INCLUDED
#define JSON_RPC_HH_INCLUDED

#include "lsp/error.hh"

namespace json_rpc {
  void add_version(picojson::object &message);
  void validate_version(picojson::object &nessage);

  picojson::object parse(const std::string &raw);
  const picojson::value &expect_field(const picojson::object &object, const std::string &name, lsp::ErrorCode errorcode);
  picojson::value get_or_null(const picojson::object &object, const std::string &name);

  template<typename T>
  const T &expect_field_t(const picojson::object &object, const std::string &name, lsp::ErrorCode errorcode)
  {
    const auto &value = expect_field(object, name, errorcode);
    try {
      return value.get<T>();
    } catch (...) {
      throw lsp::Exception(errorcode, "Invalid type for field " + name);
    }
  }

  template<typename T>
  const T &expect_t(const picojson::value &value, lsp::ErrorCode errorcode)
  {
    try {
      return value.get<T>();
    } catch (...) {
      throw lsp::Exception(errorcode, "Invalid field type");
    }
  }
}

#endif
