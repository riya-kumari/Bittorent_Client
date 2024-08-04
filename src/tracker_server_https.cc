#include "../include/tracker_server_https.h"
#include "../include/http.h"
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <iostream>
#include <istream>
#include <iterator>
#include <map>
#include <random>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include "../lib/HTTPRequest.hpp"

using namespace std;

void TrackerServer::generate_peer_id() {
    string prefix = "-TR0001-";

    random_device dev;
    mt19937 generator(dev());

    const string chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    uniform_int_distribution<mt19937::result_type> dist(0, chars.size() - 1);

    stringstream peer_id;
    peer_id << prefix;

    for (size_t i = 0; i < 20 - prefix.size(); i++) {
        peer_id << chars[dist(generator)];
    }

    this->peer_id = peer_id.str();
}

std::map<std::string, std::string> parse_bencode(const std::string& response) {
    std::map<std::string, std::string> result;
    size_t pos = 0;

    // Simple parser assuming the response is well-formed
    while (pos < response.size()) {
        char type = response[pos];
        ++pos;

        if (type == 'd') { // Dictionary
            while (response[pos] != 'e') {
                // Read key
                size_t colon_pos = response.find(':', pos);
                size_t key_len = std::stoi(response.substr(pos, colon_pos - pos));
                std::string key = response.substr(colon_pos + 1, key_len);
                pos = colon_pos + 1 + key_len;

                // Read value
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


std::vector<std::pair<std::string, int>> decode_peers(const std::string& peers) {
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

    struct TrackerRequest tracker_req;
    tracker_req.info_hash =
        "%01%8e%50%b5%81%06%b8%4a%42%c2%23%cc%f0%49%43%34%f8%d5%59%58";
    tracker_req.trackerid = this->peer_id;
    tracker_req.port = 6881;
    tracker_req.uploaded = 0;
    tracker_req.downloaded = 0;
    tracker_req.left = 658005728;
    tracker_req.events = "started";
    tracker_req.numwant = 100;
    tracker_req.compact = 0;

    std::string infoHash = "%01%8e%50%b5%81%06%b8%4a%42%c2%23%cc%f0%49%43%34%f8%d5%59%58";
    std::string peerId = "-TR0001-eTEr6BW0sHyH";
    int port = 6881;
    int bytesDownloaded = 0;
    int fileSize = 658005728;

    std::string baseUrl = "https://torrent.ubuntu.com/announce";
    std::string query = "info_hash=" + infoHash +
                        "&peer_id=" + peerId +
                        "&port=" + std::to_string(port) +
                        "&uploaded=" + std::to_string(0) +
                        "&downloaded=" + std::to_string(bytesDownloaded) +
                        "&left=" + std::to_string(fileSize - bytesDownloaded) +
                        "&event=started" +
                        "&numwant=100" +
                        "&compact=1";

    try {
        boost::system::error_code ec;
            using namespace boost::asio;

            // Required setup
            io_service svc;
            ssl::context ctx(ssl::context::sslv23_client);
            ssl::stream<ip::tcp::socket> ssock(svc, ctx);

            // Resolve the host and port
            ip::tcp::resolver resolver(svc);
            ip::tcp::resolver::query query("torrent.ubuntu.com", "443");
            ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query, ec);

            if (ec) {
                std::cerr << "Failed to resolve: " << ec.message() << std::endl;

            }

            // Connect and perform SSL handshake
            boost::asio::connect(ssock.lowest_layer(), endpoint_iterator, ec);
            if (ec) {
                std::cerr << "Failed to connect: " << ec.message() << std::endl;

            }

            ssock.handshake(ssl::stream_base::handshake_type::client, ec);
            if (ec) {
                std::cerr << "Handshake failed: " << ec.message() << std::endl;

            }

            // Send the request
            std::string request = "GET /announce?info_hash=%01%8e%50%b5%81%06%b8%4a%42%c2%23%cc%f0%49%43%34%f8%d5%59%58&peer_id=-TR0001-eTEr6BW0sHyH&port=6881&uploaded=0&downloaded=0&left=658005728&event=started&numwant=100&compact=1 HTTP/1.1\r\n";
            request += "Host: torrent.ubuntu.com\r\n";
            request += "Connection: close\r\n\r\n";

            boost::asio::write(ssock, buffer(request), ec);
            if (ec) {
                std::cerr << "Failed to send request: " << ec.message() << std::endl;

            }

            // Read the response
            std::string response;
            do {
                char buf[1024];
                size_t bytes_transferred = ssock.read_some(buffer(buf), ec);
                if (!ec) {
                    response.append(buf, buf + bytes_transferred);
                }
            } while (!ec);

            if (ec && ec != boost::asio::error::eof) {
                std::cerr << "Read error: " << ec.message() << std::endl;
            }

            // Print the response
            std::cout << "Response received: '" << response << "'\n";
            std::string resp = response;
            std::string resp_str = resp;
                  std::size_t ip_it_start;
                  int str_it = 0;

                  std::map<std::string, int> peer_clients_list;
                  // Extract Interval

                  while (resp_str.find("port", str_it) != std::string::npos)
                  {

                    // PARSING IP
                    ip_it_start = resp_str.find("ip", str_it);
                    std::size_t start_ip = resp_str.find(":", ip_it_start);
                    std::size_t end_ip = resp_str.find(":", start_ip + 1);
                    std::string ip = resp_str.substr(start_ip + 1, end_ip - start_ip - 1);

                    std::cout << "ip: " << ip << std::endl;

                    // PARSING PORT
                    str_it = resp_str.find("port", ip_it_start);
                    std::size_t port_end = resp_str.find("e", str_it);
                    int port =
                        std::stoi(resp_str.substr(str_it + 5, port_end - str_it - 5));
                    std::cout << "port: " << port << std::endl;
                    str_it = port_end;

                    peers_list[ip] = port;
                  }



    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return peers_list;
}
