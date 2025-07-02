#include "error.hh"
#include "json-rpc.hh"

picojson::object lsp::Exception::response_message(picojson::value id) const
{
  picojson::object message;
  picojson::object error;

  error["code"] = picojson::value(static_cast<double>(m_errorcode));
  error["message"] = picojson::value(m_message);

  json_rpc::add_version(message);
  message["id"] = id;
  message["error"] = picojson::value(std::move(error));

  return message;
}
