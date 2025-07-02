#include <ostream>
#include <istream>

#include "transport.hh"
#include "logging.hh"
#include "lsp/error.hh"

void transport::send(const std::string &content, std::ostream &os)
{
  std::size_t length = content.size();
  os << "Content-Length: " << length << "\r\n";
  os << "Content-Type:application/vscode-jsonrpc;charset=utf-8" << "\r\n";
  os << "\r\n";
  os << content;
  log_::debug("transport::send >> %s", content.c_str());
}

std::string transport::recv(std::istream &is)
{
  std::string line;
  std::size_t length = 0;
  bool found_length = false;

  while (true) {
    char c = is.get();

    if (c == '\r') {
      c = is.get();

      if (c != '\n') {
        throw lsp::Exception(lsp::ErrorCode::InvalidRequest,
                             "Invalid delimiter in message header");
      }

      if (line.empty()) {
        break;
      }

      log_::debug("Received header '%s'", line.c_str());

      const std::string::size_type sep = line.find(":");
      const std::string key = line.substr(0, sep);
      const std::string value = line.substr(sep + 1);

      if (key == "Content-Length") {
        length = std::stoi(value);
        found_length = true;
      } else if (key == "Content-Type") {
        if (value != "application/vscode-jsonrpc;charset=utf-8") {
          throw lsp::Exception(lsp::ErrorCode::InvalidRequest,
                               "Unsupported Content-Type " + value);
        }
      } else {
        throw lsp::Exception(lsp::ErrorCode::InvalidRequest,
                             "Unsupported header key '" + key + "'");
      }

      line.clear();

      continue;
    }

    line.push_back(c);
  }

  if (!found_length) {
    throw lsp::Exception(lsp::ErrorCode::InvalidRequest,
                         "Missing Content-Length header");
  }

  std::string content(length, 0);
  is.read(&content[0], length);

  log_::debug("transport::recv << %s", content.c_str());

  return content;
}
