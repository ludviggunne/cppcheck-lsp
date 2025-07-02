#include "cppcheck.hh"

const char **cppcheck::Config::argv(const std::string &file)
{
  m_argv.clear();
  m_argv_c_str.clear();

  m_argv.push_back(cppcheck_path);

  m_argv.push_back("-q");
  m_argv.push_back(std::string("--template='") + ErrorMessage::error_template() + "'");

  m_argv.insert(m_argv.end(), extra_args.cbegin(), extra_args.cend());
  m_argv.push_back(file);

  for (const auto &str : m_argv) {
    m_argv_c_str.push_back(str.c_str());
  }

  m_argv_c_str.push_back(nullptr);

  return m_argv_c_str.data();
}

const char *cppcheck::ErrorMessage::error_template()
{
  return "{file}|{line}|{column}|{severity}|{message}";
}

bool cppcheck::ErrorMessage::parse(std::string message_line)
{
  std::vector<std::string> components;
  std::string::size_type pos;

  /* Tokenize */
  while ((pos = message_line.find("|")) != std::string::npos) {
    components.push_back(message_line.substr(0, pos));
    message_line = message_line.substr(pos + 1);

    /* Reached message string */
    if (components.size() == 4)
      break;
  }

  /* Add message */
  components.push_back(message_line);

  if (components.size() != 5) {
    return false;
  }

  file = components[0];

  try {
    line = std::stoi(components[1]);
    column = std::stoi(components[2]);
  } catch (...) {
    return false;
  }

  /* LSP is zero-based, cppcheck is one-based */
  if (line > 0) {
    line--;
  }
  if (column > 0) {
    column--;
  }

  severity = components[3];

  if (severity == "error") {
    lsp_severity = lsp::Severity::Error;
  } else if (severity == "warning") {
    lsp_severity = lsp::Severity::Warning;
  } else if (severity == "style") {
    lsp_severity = lsp::Severity::Hint;
  } else if (severity == "performance") {
    lsp_severity = lsp::Severity::Hint;
  } else if (severity == "portability") {
    lsp_severity = lsp::Severity::Hint;
  } else if (severity == "information") {
    lsp_severity = lsp::Severity::Information;
  } else {
    return false;
  }

  message = components[4];

  return true;
}

picojson::value cppcheck::ErrorMessage::into_lsp_diagnostic() const
{
  picojson::object start, end;
  start["line"] = picojson::value(static_cast<double>(line));
  start["character"] = picojson::value(static_cast<double>(column));

  end["line"] = picojson::value(static_cast<double>(line));
  end["character"] = picojson::value(static_cast<double>(column + 1));

  picojson::object range;
  range["start"] = picojson::value(std::move(start));
  range["end"] = picojson::value(std::move(end));

  picojson::object diagnostic;
  diagnostic["range"] = picojson::value(std::move(range));
  diagnostic["message"] = picojson::value(message);
  diagnostic["severity"] = picojson::value(static_cast<double>(lsp_severity));

  return picojson::value(std::move(diagnostic));
}

