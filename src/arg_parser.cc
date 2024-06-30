#include "../include/arg_parser.h"
#include <argp.h>

using namespace std;


static int parse_opt(int key, char *arg, struct argp_state *state) {
    struct args *arguments = (struct args *) state->input;

    switch (key) {
        case 'f':
            arguments->torrent_file = arg;
            break;
    }
    return 0;
}

struct args torrent_parseopt(int argc, char *argv[]) {
    struct args arguments = {""};

    struct argp_option options[] = {
        {"torrent_file", 'f', "torrent_file", 0, "The bittorrent file that is being downloaded.", 0},
        {0}
    };
    struct argp argp = {options, parse_opt, 0, 0};
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    return arguments;
}
