CXX = g++ --std=c++17
CXXFLAGS = -Wall -Werror -ggdb
CXXCPPFLAGS = -I./lib

all: client

client: src/client.cc src/arg_parser.cc
	@echo "Building client"
	$(CXX) $(CXXFLAGS) $(CXXCPPFLAGS) -o $@ $^

clean:
	rm -f client src/*.o

.PHONY: clean all
