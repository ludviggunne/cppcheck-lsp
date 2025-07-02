#ifndef SERVER_HH_INCLUDED
#define SERVER_HH_INCLUDED

#include <iostream>
#include <picojson/picojson.h>

#include "cppcheck.hh"

class Server {
public:
  Server(std::ostream &ostream, std::istream &istream, cppcheck::Config &&config)
    : m_initialized(false)
    , m_did_shutdown(false)
    , m_did_exit(false)
    , m_ostream(ostream)
    , m_istream(istream)
    , m_config(config)
  {
  }

  int run_loop();

private:
  picojson::object recv() const;
  void send(picojson::object &&message);

  picojson::object handle_message(const picojson::object &&message);

  picojson::object handle_initialize_request(const picojson::object &&params);
  picojson::object handle_textdocument_did_open_notification(const picojson::object &&params);
  picojson::object handle_textdocument_did_save_notification(const picojson::object &&params);

private:
  bool m_initialized;
  bool m_did_shutdown;
  bool m_did_exit;
  std::ostream &m_ostream;
  std::istream &m_istream;
  picojson::value m_current_request_id;
  cppcheck::Config m_config;
};

#endif
