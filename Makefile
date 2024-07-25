CXX = g++ --std=c++17
CXXFLAGS = -Wall -Werror -ggdb
LDLIBS = -lssl -lcrypto
CXXCPPFLAGS = -I./include

all: bittorent

bittorent: src/main.cc src/arg_parser.cc src/tracker_server.cc src/http.cc src/messages.cc src/client.cc
	@echo "Building main"
	$(CXX) $(CXXFLAGS) $(CXXCPPFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -f client src/*.o

.PHONY: clean all
