#include "lsp/error.hh"
#include "options.hh"
#include "version.hh"

static const char *s_help =
  "cppcheck-lsp version " CPPCHECK_LSP_VERSION "\n"
  "Usage: cppcheck-lsp [OPTIONS...] [-- [CPPCHECK OPTIONS]]\n"
  "\n"
  "Available options:\n"
  "    --log-file=<file>         Specify output file for logging. Default is stderr.\n"
  "    --log-level=<level>       Specify log level. May be one of:\n"
  "                                * none (default)\n"
  "                                * error\n"
  "                                * info\n"
  "                                * debug\n"
  "    --cppcheck-path=<path>    Specify path to cppcheck executable.\n"
  "    --help                    Show this help message and exit.\n"
  "\n"
  "Any options appearing after '--' will be passed on to cppcheck.\n"
  "";

void Options::show_help(std::ostream &out)
{
  out << s_help << std::endl;
}

void Options::parse_cli(int argc, char **argv)
{
  (void) argc;
  argv++;

  for (; *argv; argv++) {
    std::string arg = *argv;

    if (arg == "--") {
      argv++;
      break;
    }

    if (arg.rfind("--", 0) != std::string::npos) {
      long_option(arg);
      continue;
    }

    throw lsp::Exception(lsp::ErrorCode::InternalError, "Invalid cli argument " + arg);
  }

  for (; *argv; argv++) {
    cppcheck_args.push_back(*argv);
  }
}

void Options::long_option(const std::string &arg)
{
  const std::string::size_type eq = arg.find("=");
  const bool has_arg = eq != std::string::npos;

  const std::string option = has_arg ? arg.substr(0, eq) : arg;
  const std::string optarg = has_arg ? arg.substr(eq + 1) : "";

  if (option == "--log-file") {
    if (optarg.empty()) {
      throw lsp::Exception(lsp::ErrorCode::InternalError, "Option --log-file=... requires an argument");
    }

    logfile = optarg;

  } else if (option == "--log-level") {
    if (optarg.empty()) {
      throw lsp::Exception(lsp::ErrorCode::InternalError, "Option --log-level=... requires an argument");
    }

    if (optarg == "none") {
      loglevel = log_::Level::None;
    } else if (optarg == "error") {
      loglevel = log_::Level::Error;
    } else if (optarg == "info") {
      loglevel = log_::Level::Info;
    } else if (optarg == "debug") {
      loglevel = log_::Level::Debug;
    } else {
      throw lsp::Exception(lsp::ErrorCode::InternalError, "Invalid log level " + optarg);
    }

  } else if (option == "--cppcheck-path") {
    if (optarg.empty()) {
      throw lsp::Exception(lsp::ErrorCode::InternalError, "Option --cppcheck-path=... requires an argument");
    }

    cppcheck_path = optarg;

  } else if (option == "--help") {
    show_help(std::cout);
    std::exit(0);

  } else {
    throw lsp::Exception(lsp::ErrorCode::InternalError, "Invalid cli argument " + arg);
  }
}
