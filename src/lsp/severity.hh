#ifndef LSP_SEVERITY_INCLUDED
#define LSP_SEVERITY_INCLUDED

namespace lsp {

  enum class Severity : unsigned int {
	  Error= 1,
	  Warning= 2,
	  Information= 3,
	  Hint= 4,
  };

};

#endif
