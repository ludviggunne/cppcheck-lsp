#ifndef LSP_ERROR_HH_INCLUDED
#define LSP_ERROR_HH_INCLUDED

#include <cstdint>
#include <exception>
#include <string>
#include <picojson/picojson.h>

namespace json_rpc {
  enum class ErrorCode : std::int32_t {
    ParseError     = -32700,
	  InvalidRequest = -32600,
	  MethodNotFound = -32601,
	  InvalidParams  = -32602,
	  InternalError  = -32603,
  };
}

namespace lsp {

  enum class ErrorCode : std::int32_t {
    ParseError                     = static_cast<std::int32_t>(json_rpc::ErrorCode::ParseError),
	  InvalidRequest                 = static_cast<std::int32_t>(json_rpc::ErrorCode::InvalidRequest),
	  MethodNotFound                 = static_cast<std::int32_t>(json_rpc::ErrorCode::MethodNotFound),
	  InvalidParams                  = static_cast<std::int32_t>(json_rpc::ErrorCode::InvalidParams),
	  InternalError                  = static_cast<std::int32_t>(json_rpc::ErrorCode::InternalError),
    jsonrpcReservedErrorRangeStart = -32099,
    serverErrorStart               = jsonrpcReservedErrorRangeStart,
    ServerNotInitialized           = -32002,
    UnknownErrorCode               = -32001,
    jsonrpcReservedErrorRangeEnd   = -32000,
    serverErrorEnd                 = jsonrpcReservedErrorRangeEnd,
    lspReservedErrorRangeStart     = -32899,
    RequestFailed                  = -32803,
    ServerCancelled                = -32802,
    ContentModified                = -32801,
    RequestCancelled               = -32800,
    lspReservedErrorRangeEnd       = -32800,
  };

  class Exception : public std::exception {
  public:
    Exception(ErrorCode errorcode, const std::string &&message)
      : m_errorcode(errorcode)
      , m_message(std::move(message))
    {
    }

    virtual const char *what() const noexcept override
    {
      return m_message.c_str();
    }

    /* Create a response message corresponding to this exception
     * with the provided request id */
    picojson::object response_message(picojson::value id = picojson::value()) const;

  private:
    ErrorCode m_errorcode;
    std::string m_message;
  };
};

#endif
