CXX = g++ --std=c++17
CXXFLAGS = -Wall -Werror -ggdb
LDLIBS = -lssl -lcrypto -lboost_system -lboost_filesystem -I/usr/local/include -L/usr/local/lib64/
CXXCPPFLAGS = -I./include

all: bittorent

bittorent: src/main.cc src/arg_parser.cc src/tracker_server_https.cc src/http.cc src/messages.cc src/client.cc
	@echo "Building main"
	$(CXX) $(CXXFLAGS) $(CXXCPPFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -f bittorent src/*.o

.PHONY: clean all
