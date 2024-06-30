#include "../include/arg_parser.h"
#include "../lib/bencode.hpp"

#include <argp.h>
#include <iostream>

using namespace std;


int main(int argc, char *argv[]) {
    struct args arguments = torrent_parseopt(argc, argv);

    if (!arguments.torrent_file.empty()) {
        cout << "Torrent file: " << arguments.torrent_file << endl;
    } else {
        cout << "No torrent file provided." << endl;
    }

    // Variables
    // bencode::data data;
    // data = bencode::decode("i42e");
    // auto value = std::get<bencode::integer>(data);
    // std::cout << "Here : " << value << std::endl;

    return 0;
}
