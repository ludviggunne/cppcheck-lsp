#ifndef LSP_MESSAGE_HH
#define LSP_MESSAGE_HH

#include <cstdint>

#include "json-rpc.hh"

namespace lsp {
  picojson::object create_response_message(std::int64_t id, picojson::object &&result, picojson::object &&error);
  picojson::object create_notification_message(std::int64_t, std::string &&method, picojson::object &&params);
};

#endif
