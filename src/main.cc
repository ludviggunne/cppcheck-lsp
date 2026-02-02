#include <iostream>
#include <cstdio>

#include "server.hh"
#include "transport.hh"
#include "json-rpc.hh"
#include "logging.hh"
#include "posix.hh"
#include "options.hh"
#include "lsp/error.hh"

int main(int argc, char **argv)
{
  Options options;

  try {
    options.parse_cli(argc, argv);

    log_::enable(options.loglevel);
   
    if (!options.logfile.empty()) {
      log_::set_file(options.logfile);
    }
  } catch (const lsp::Exception &exception) {

    if (posix::isatty(posix::fileno(stdout))) {
      std::cerr << "Error: " << exception.what() << std::endl;
      Options::show_help(std::cerr);

    } else {
      picojson::object message = exception.response_message(picojson::value(nullptr));
      json_rpc::add_version(message);
      std::string content = picojson::value(std::move(message)).serialize();
      transport::send(content, std::cout);
    }

    std::exit(1);
  }

  cppcheck::Config config;
  config.extra_args = std::move(options.cppcheck_args);
  config.cppcheck_path = options.cppcheck_path;

  Server server(std::cout, std::cin, std::move(config));
  server.run_loop();
}
