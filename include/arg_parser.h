#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include <string>
#include <argp.h>

struct args {
    std::string torrent_file;
};

// Function declaration for argument parsing
struct args torrent_parseopt(int argc, char *argv[]);

#endif // ARG_PARSER_H
