#include "../include/arg_parser.h"
#include "../lib/bencode.hpp"
#include <argp.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <openssl/sha.h>
#include <sstream>
#include <typeinfo>
#include "../include/tracker_server.h"

#define PRINT true
// https://stackoverflow.com/questions/6934232/is-there-a-simple-c-or-c-function-that-calculates-the-sha1-hash-of-a-string
unsigned char *create_hash(const unsigned char *str, size_t length) {
  unsigned char *hash = (unsigned char *)malloc(SHA_DIGEST_LENGTH);
  SHA1(str, length, hash);
  return hash;
}

int main(int argc, char *argv[]) {
  // Parse path to torrent file from arguments
  struct args arguments = torrent_parseopt(argc, argv);

  if (!arguments.torrent_file.empty()) {
    std::cout << "Torrent file: " << arguments.torrent_file << std::endl;
  } else {
    std::cout << "No torrent file provided." << std::endl;
  }

  // Read the torrent file as a string
  // ifstream is a standard library class for input file stream operations
  std::ifstream file(arguments.torrent_file); // taking file as inputstream
  std::string torrent_file_str;
  if (file) {
    std::ostringstream ss;
    ss << file.rdbuf(); // reading data
    torrent_file_str = ss.str();
  }

  // Decode the torrent file content using bencode
  bencode::data data;
  bencode::dict info_hash;
  std::string info_hash_str;
  std::string announce_url;
  bool url_extracted{false};
  bool info_extracted{false};
  try {
    bencode::data decode_data = bencode::decode(torrent_file_str);

    // bencode lib should decode the file into a dictionary
    // we need tracker url and info hash from the torrent file
    // https://stackoverflow.com/questions/1443793/iterate-keys-in-a-c-map
    if (auto *data_dict = std::get_if<bencode::dict>(&decode_data)) {
      for (auto &[key, val] : *data_dict) {

        // extract tracker server url
        if (auto url = std::get_if<std::string>(&val)) {
          if (!url_extracted) {
            announce_url = *url;
#ifdef PRINT
            std::cout << "Announce URL: " << announce_url << std::endl;
#endif
            url_extracted = true;
          }
        }

        // extract info hash
        if (auto hash = std::get_if<bencode::dict>(&val)) {
          if (!info_extracted) {
            // info_hash = hash.copy();
            info_hash.insert(hash->begin(),
                             hash->end()); // copy hash to info_hash

            // Encode the info dictionary back to a string
            std::string encoded_hash = bencode::encode(info_hash);

            // Compute SHA-1 hash
            unsigned char *hash = create_hash(
                (unsigned char *)encoded_hash.c_str(), encoded_hash.length());

            std::stringstream formattedString;
            for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
              formattedString << "%" << std::setw(2) << std::setfill('0')
                              << std::hex << static_cast<int>(hash[i]);
            }
            info_hash_str = formattedString.str();
#ifdef PRINT
            std::cout << "Info Hash : " << info_hash_str << std::endl;
#endif
            info_extracted = true;
          }
        }
      }

    } else {
      std::cerr
          << "[Error] Something is wrong with the format of the torrent file.";
    }

    TrackerServer tracker(info_hash_str);
    tracker.tracker_req_str = announce_url;
    auto peers_list = tracker.create_tracker_req();

    std::cout << "Clients list : " << std::endl;
    for(const auto& [ip,port]: peers_list){
        std::cout << "ip" << ip << ", port : " << port <<std::endl;
    }


  } catch (const std::exception &e) {
    std::cerr << "Error decoding torrent file: " << e.what() << std::endl;
    return 1;
  }

  return 0;

}
