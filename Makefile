PREFIX=.
override CXXFLAGS:=-MMD -Isrc -Iexternal -O3 $(CXXFLAGS)
override LDFLAGS:=-flto $(LDFLAGS)

common_sources=src/transport.cc \
               src/json-rpc.cc \
               src/server.cc \
               src/lsp/error.cc \
               src/logging.cc \
               src/posix.cc \
               src/options.cc \
               src/subprocess.cc \
               src/cppcheck.cc
main_sources=src/main.cc
test_sources=src/test/testing.cc \
             src/test/main.cc \
             src/test/transport.cc \
             src/test/json-rpc.cc \
             src/test/subprocess.cc \
             src/test/cppcheck.cc

common_objects=$(common_sources:%.cc=%.o)
common_dependencies=$(common_sources:%.cc=%.d)
main_objects=$(main_sources:%.cc=%.o)
main_dependencies=$(main_sources:%.cc=%.d)
test_objects=$(test_sources:%.cc=%.o)
test_dependencies=$(test_sources:%.cc=%.d)

all: cppcheck-lsp test-cppcheck-lsp

cppcheck-lsp: $(common_objects) $(main_objects)
	$(CXX) $(LDFLAGS) -o $(@) $(^)

test-cppcheck-lsp: $(common_objects) $(test_objects)
	$(CXX) $(LDFLAGS) -o $(@) $(^)

-include $(common_dependencies) $(main_dependencies) $(test_dependencies)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -o $(@) -c $(<)

clean:
	rm -f cppcheck-lsp \
		$(common_objects) $(common_dependencies) \
		$(main_objects) $(main_dependencies) \
		$(test_objects) $(test_dependencies)

install:
	install -Dm755 cppcheck-lsp $(PREFIX)/bin/cppcheck-lsp

.PHONY: clean install
.DEFAULT_GOAL=cppcheck-lsp
