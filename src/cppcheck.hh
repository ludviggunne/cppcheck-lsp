#ifndef CPPCHECK_HH_INCLUDED
#define CPPCHECK_HH_INCLUDED

#include <picojson/picojson.h>
#include <string>

#include "lsp/severity.hh"

namespace cppcheck {

  struct Config {
    const char **argv(const std::string &file);

    std::string cppcheck_path = "cppcheck";
    std::vector<std::string> extra_args;

  private:
    std::vector<std::string> m_argv;
    std::vector<const char *> m_argv_c_str;
  };

  struct ErrorMessage {

    std::string file;
    unsigned int line;
    unsigned int column;
    unsigned int token_length;
    std::string severity;
    lsp::Severity lsp_severity;
    std::string message;
    std::string code;

    static const char *error_template();
    bool parse(std::string message_line);
    picojson::value into_lsp_diagnostic() const;
  };
};

#endif
