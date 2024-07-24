CXX = g++ --std=c++17
CXXFLAGS = -Wall -Werror -ggdb
LDLIBS = -lssl -lcrypto
CXXCPPFLAGS = -I./include

all: client

client: src/client.cc src/arg_parser.cc src/tracker_server.cc src/http.cc
	@echo "Building client"
	$(CXX) $(CXXFLAGS) $(CXXCPPFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -f client src/*.o

.PHONY: clean all
