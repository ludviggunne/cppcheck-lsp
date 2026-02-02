#include "server.hh"
#include "transport.hh"
#include "json-rpc.hh"
#include "lsp/error.hh"
#include "version.hh"
#include "logging.hh"
#include "cppcheck.hh"
#include "subprocess.hh"

int Server::run_loop()
{
  log_::info("Server started");

  for (;;) {
    try {
      const auto message = recv();
      auto response = handle_message(std::move(message));

      if (m_did_exit) {
        log_::info("Exiting...");
        return m_did_shutdown ? 0 : 1;
      }

      if (!response.empty()) {
        send(std::move(response));
      }
    } catch (const lsp::Exception &exception) {;

      log_::error("Exception caught: %s", exception.what());
      send(exception.response_message(m_current_request_id));

      if (!m_initialized) {
        std::exit(1);
      }
    }
  }
}

picojson::object Server::recv() const
{
  const std::string raw = transport::recv(m_istream);
  picojson::object message = json_rpc::parse(raw);
  return message;
}

void Server::send(picojson::object &&message)
{
  json_rpc::add_version(message);
  std::string raw = picojson::value(std::move(message)).serialize();
  transport::send(raw, m_ostream);
}

picojson::object Server::handle_message(const picojson::object &&message)
{
  if (message.count("id") == 0) {
    m_current_request_id = picojson::value();
  } else {
    m_current_request_id = message.at("id");
  }

  const auto method = json_rpc::expect_field_t<std::string>(message, "method", lsp::ErrorCode::InvalidRequest);
  const auto params = json_rpc::get_or_null(message, "params");

  if (m_current_request_id.is<picojson::null>()) {
    log_::info("Received '%s' notification", method.c_str());
  } else {
    log_::info("Received '%s' request", method.c_str());
  }

  /* Make sure the first request is an initialize request */
  if (method != "initialize" && !m_initialized) {
    throw lsp::Exception(lsp::ErrorCode::ServerNotInitialized, "Excepted method 'initialize'");
  }

  if (method == "initialize") {
    const auto params_object = json_rpc::expect_t<picojson::object>(std::move(params), lsp::ErrorCode::InvalidRequest);
    return handle_initialize_request(std::move(params_object));

  } else if (method == "initialized") {
    return picojson::object();

  } else if (method == "textDocument/didOpen") {
    const auto params_object = json_rpc::expect_t<picojson::object>(std::move(params), lsp::ErrorCode::InvalidRequest);
    return handle_textdocument_did_open_notification(std::move(params_object));

  } else if (method == "textDocument/didSave") {
    const auto params_object = json_rpc::expect_t<picojson::object>(std::move(params), lsp::ErrorCode::InvalidRequest);
    return handle_textdocument_did_save_notification(std::move(params_object));

  } else if (method == "shutdown") {
    m_did_shutdown = true;

    picojson::object response;
    response["id"] = m_current_request_id;
    response["result"] = picojson::value();

    json_rpc::add_version(response);

    return response;

  } else if (method == "exit") {
    m_did_exit = true;
    return picojson::object();

  } else {
    log_::error("Unsupported method '%s'", method.c_str());
  }

  return picojson::object();
}
static picojson::value get_server_capabilities()
{
  picojson::object diagnostic_options;
  diagnostic_options["interFileDependencies"] = picojson::value(false);
  diagnostic_options["workspaceDiagnostics"] = picojson::value(false);

  picojson::object capabilities;
  capabilities["diagnosticProvider"] = picojson::value(std::move(diagnostic_options));

  /* Disable document sync, cppcheck reads from disk */
  capabilities["textDocumentSync"] = picojson::value(0.0);

  return picojson::value(std::move(capabilities));
}

static picojson::value get_server_info()
{
  picojson::object server_info;
  server_info["name"] = picojson::value("cppcheck-lsp");
  server_info["version"] = picojson::value(CPPCHECK_LSP_VERSION);
  return picojson::value(std::move(server_info));
}

picojson::object Server::handle_initialize_request(const picojson::object &&params)
{
  // TODO: check client capabilities
  // TODO: save workspaceFolders?
  (void) params;

  picojson::object result;
  result["capabilities"] = get_server_capabilities();
  result["serverInfo"] = get_server_info();

  picojson::object response;
  response["id"] = m_current_request_id;
  response["result"] = picojson::value(std::move(result));

  m_initialized = true;
  return response;
}

picojson::object Server::handle_textdocument_did_open_notification(const picojson::object &&params)
{
  return handle_textdocument_did_save_notification(std::move(params));
}

picojson::object Server::handle_textdocument_did_save_notification(const picojson::object &&params)
{
  const picojson::object text_document = json_rpc::expect_field_t<picojson::object>(
    params, "textDocument", lsp::ErrorCode::InvalidRequest);

  const std::string uri = json_rpc::expect_field_t<std::string>(
    text_document, "uri", lsp::ErrorCode::InvalidRequest);

  const std::string file = uri.substr(std::strlen("file://"));
  const auto argv = m_config.argv(file);
  const auto result = subprocess::run(argv);

  if (result.exitcode != 0) {
    log_::error("Failed to run %s (exit code %d): %s", argv[0], result.exitcode, result.output[0].c_str());
    throw lsp::Exception(lsp::ErrorCode::RequestFailed, "Failed to run cppcheck");
  }

  picojson::array diagnostics;
  cppcheck::ErrorMessage message;

  for (auto &line : result.output) {
    log_::debug("%s", line.c_str());
    if (!message.parse(line)) {
      log_::error("Failed to parse '%s'", line.c_str());
      continue;
    }
    diagnostics.push_back(picojson::value(message.into_lsp_diagnostic()));
  }

  picojson::object notification_params;
  notification_params["uri"] = picojson::value(std::move(uri));
  notification_params["diagnostics"] = picojson::value(std::move(diagnostics));

  picojson::object notification;
  notification["method"] = picojson::value("textDocument/publishDiagnostics");
  notification["params"] = picojson::value(std::move(notification_params));

  json_rpc::add_version(notification);

  return notification;
}
