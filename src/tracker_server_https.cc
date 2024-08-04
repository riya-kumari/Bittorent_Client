#include "../include/tracker_server_https.h"
#include "../include/http.h"
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <iomanip>
#include <iostream>
#include <istream>
#include <iterator>
#include <map>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "../lib/HTTPRequest.hpp"
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>

using namespace std;

void TrackerServer::generate_peer_id() {
    string prefix = "-TR0001-";

    random_device dev;
    mt19937 generator(dev());

    const string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    uniform_int_distribution<mt19937::result_type> dist(0, chars.size() - 1);

    stringstream peer_id;
    peer_id << prefix;

    for (size_t i = 0; i < 20 - prefix.size(); i++) {
        peer_id << chars[dist(generator)];
    }

    this->peer_id = peer_id.str();
}

std::map<std::string, std::string> parse_bencode(const std::string &response) {
    std::map<std::string, std::string> result;
    size_t pos = 0;

    while (pos < response.size()) {
        char type = response[pos];
        ++pos;

        if (type == 'd') { // Dictionary
            while (response[pos] != 'e') {
                size_t colon_pos = response.find(':', pos);
                size_t key_len = std::stoi(response.substr(pos, colon_pos - pos));
                std::string key = response.substr(colon_pos + 1, key_len);
                pos = colon_pos + 1 + key_len;

                char value_type = response[pos];
                ++pos;

                if (value_type == 'i') { // Integer
                    size_t e_pos = response.find('e', pos);
                    std::string value = response.substr(pos, e_pos - pos);
                    result[key] = value;
                    pos = e_pos + 1;
                } else if (value_type == 'l' || value_type == 'd') {
                    throw std::runtime_error("Nested lists or dictionaries are not supported in this parser.");
                } else if (std::isdigit(value_type)) { // String
                    size_t colon_pos = response.find(':', pos);
                    size_t value_len = std::stoi(response.substr(pos, colon_pos - pos));
                    std::string value = response.substr(colon_pos + 1, value_len);
                    result[key] = value;
                    pos = colon_pos + 1 + value_len;
                } else {
                    throw std::runtime_error("Unexpected value type.");
                }
            }
            ++pos; // Skip 'e'
        } else {
            throw std::runtime_error("Invalid format.");
        }
    }

    return result;
}

std::vector<std::pair<std::string, int>> decode_peers(const std::string &peers) {
    std::vector<std::pair<std::string, int>> peer_list;
    for (size_t i = 0; i < peers.size(); i += 6) {
        std::string ip = std::to_string((unsigned char)peers[i]) + "." +
                         std::to_string((unsigned char)peers[i + 1]) + "." +
                         std::to_string((unsigned char)peers[i + 2]) + "." +
                         std::to_string((unsigned char)peers[i + 3]);
        int port = ((unsigned char)peers[i + 4] << 8) | (unsigned char)peers[i + 5];
        peer_list.emplace_back(ip, port);
    }
    return peer_list;
}

map<string, int> TrackerServer::create_tracker_req() {
    std::map<std::string, int> peers_list;
    this->generate_peer_id();

    std::string TRACKER_URL = "torrent.ubuntu.com";
    std::string TRACKER_PORT = "443"; // HTTPS port

    std::string infoHash = "%01%8e%50%b5%81%06%b8%4a%42%c2%23%cc%f0%49%43%34%f8%d5%59%58";
    std::string peerId = this->peer_id;
    int port = 6881;
    int bytesDownloaded = 0;
    int fileSize = 658005728;

    std::string query = "info_hash=" + infoHash + "&peer_id=" + peerId +
                        "&port=" + std::to_string(port) +
                        "&uploaded=0&downloaded=" + std::to_string(bytesDownloaded) +
                        "&left=" + std::to_string(fileSize - bytesDownloaded) +
                        "&event=started&numwant=100&compact=1";

    std::string request =
        "GET /announce?" + query + " HTTP/1.1\r\n"
        "Host: " + TRACKER_URL + "\r\n"
        "Connection: close\r\n\r\n";

    try {
        boost::system::error_code ec;
        using namespace boost::asio;

        io_service svc;
        ssl::context ctx(ssl::context::sslv23_client);
        ssl::stream<ip::tcp::socket> ssock(svc, ctx);

        // Resolve the host and port
        ip::tcp::resolver resolver(svc);
        ip::tcp::resolver::query query_resolver(TRACKER_URL, TRACKER_PORT);
        ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query_resolver, ec);

        if (ec) {
            std::cerr << "Failed to resolve: " << ec.message() << std::endl;
            return peers_list;
        }

        // Connect and perform SSL handshake
        boost::asio::connect(ssock.lowest_layer(), endpoint_iterator, ec);
        if (ec) {
            std::cerr << "Failed to connect: " << ec.message() << std::endl;
            return peers_list;
        }

        ssock.handshake(ssl::stream_base::handshake_type::client, ec);
        if (ec) {
            std::cerr << "Handshake failed: " << ec.message() << std::endl;
            return peers_list;
        }

        // Send the request
        boost::asio::write(ssock, buffer(request), ec);
        if (ec) {
            std::cerr << "Failed to send request: " << ec.message() << std::endl;
            return peers_list;
        }

        // Read the response
        boost::asio::streambuf response_buffer;
        boost::system::error_code read_error;
        std::ostringstream response_stream;

        do {
            boost::asio::read(ssock, response_buffer, boost::asio::transfer_at_least(1), read_error);
            response_stream << &response_buffer;
        } while (!read_error && read_error != boost::asio::error::eof);

        if (read_error && read_error != boost::asio::error::eof) {
            std::cerr << "Read error: " << read_error.message() << std::endl;
            return peers_list;
        }

        std::string response = response_stream.str();

        // Extract peers from response
        size_t start = response.find("peers") + 6; // "peers" key and "0:" prefix
        size_t end = response.find("e", start); // Find end of peers section
        std::string peers_str = response.substr(start, end - start);


        for (size_t i = 0; i < peers_str.size(); i += 6) {
            std::string ip = std::to_string((unsigned char)peers_str[i]) + "." +
                             std::to_string((unsigned char)peers_str[i + 1]) + "." +
                             std::to_string((unsigned char)peers_str[i + 2]) + "." +
                             std::to_string((unsigned char)peers_str[i + 3]);
            int port = ((unsigned char)peers_str[i + 4] << 8) | (unsigned char)peers_str[i + 5];
            peers_list[ip]= port;
        }
        // std::vector<std::pair<std::string, int>> peers = decode_peers(peers_str);

        // for (const auto& [ip, port] : peers) {
        //     peers_list[ip] = port;
        // }

    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return peers_list;
}
