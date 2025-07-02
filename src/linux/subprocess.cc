#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/signalfd.h>
#include <poll.h>
#include <string.h>
#include <errno.h>

#include "subprocess.hh"
#include "lsp/error.hh"

static void throw_with_strerror(const std::string &function)
{
  std::string message = function + " failed: " + strerror(errno);
  throw lsp::Exception(lsp::ErrorCode::InternalError, std::move(message));
}

subprocess::Result subprocess::run(const char **argv)
{
  subprocess::Result result { 0 };

  int pipe_fds[2];
  if (pipe(pipe_fds) < 0) {
    throw_with_strerror("pipe");
  }

  int read_fd = pipe_fds[0];
  int write_fd = pipe_fds[1];

  int pid = fork();
  if (pid < 0) {
    throw_with_strerror("fork");
  }

  if (pid == 0) {

    dup2(write_fd, STDOUT_FILENO);
    dup2(write_fd, STDERR_FILENO);
    close(read_fd);

    execvp(argv[0], const_cast<char *const *>(argv));

    std::cout << "execvp failed: " << strerror(errno) << std::endl;
    std::exit(1);
  }

  close(write_fd);

  FILE *subprocess_file = fdopen(read_fd, "r");
  if (subprocess_file == NULL) {
    throw_with_strerror("fdopen(pipe)");
  }

  char *line = NULL;
  std::size_t size;

  for (;;) {

    if (getline(&line, &size, subprocess_file) < 0) {
      break;
    }

    /* Remove trailing newline */
    std::size_t length = strlen(line);
    if (line[length - 1] == '\n') {
      line[length - 1] = 0;
    }

    result.output.push_back(line);
  }

  int status;
  if (wait(&status) < 0) {
    throw_with_strerror("wait");
  }

  if (WIFEXITED(status)) {
    result.exitcode = WEXITSTATUS(status);
  } else {
    result.exitcode = 1;
  }

  return result;
}
