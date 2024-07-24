#include "../include/tracker_server.h"
#include <iostream>
#include <map>
#include <random>
#include <sstream> // Add this include
#include "../include/http.h"

using namespace std;

string TrackerServer::generate_peer_id() {
  string prefix = "-PC0001-";

  random_device dev;
  mt19937 generator(dev());
  mt19937 rng(dev());

  const string chars =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  std::uniform_int_distribution<std::mt19937::result_type> dist(
      0, chars.size() - 1);

  stringstream peer_id;
  peer_id << prefix;

  // 20 is the length of the peer id
  for (size_t i = 0; i < 20 - prefix.size(); i++) {
    peer_id << chars[dist(generator)];
  }

  return peer_id.str();

  // # -<2 character id><4 digit version number>-<random numbers>
  // >>> '-PC0001-' + ''.join([str(random.randint(0, 9)) for _ in range(12)])
  // '-PC0001-478269329936'   //
}

map<string, int> TrackerServer::create_tracker_req() {
  std::map<std::string, int> peers_list;
  string peer_id = generate_peer_id();


  std::string TRACKER_URL = this->tracker_req_str.erase(this->tracker_req_str.size() - 14);
  struct TrackerRequest tracker_req;
  tracker_req.info_hash = this->info_hash;
  tracker_req.trackerid = peer_id;
  tracker_req.port = 6881;
  tracker_req.uploaded = 0;
  tracker_req.downloaded = 0;
  tracker_req.left = 658005728;
  tracker_req.events = "started";
  tracker_req.numwant = 100;
  tracker_req.no_peer_id = 0;
  tracker_req.compact = 0;

  std::string tracker_req_str =
      "GET /announce?info_hash=" + tracker_req.info_hash +
      "&peer_id=" + tracker_req.trackerid +
      "&port=" + std::to_string(tracker_req.port) +
      "&uploaded=" + std::to_string(tracker_req.uploaded) +
      "&downloaded=" + std::to_string(tracker_req.downloaded) +
      "&left=" + std::to_string(tracker_req.left) +
      "&event=" + tracker_req.events +
      "&numwant=" + std::to_string(tracker_req.numwant) + "%compact" +
      std::to_string(tracker_req.compact) + " HTTP/1.1 \r\n\r\n";

  // std::cout << "tracker req :" << tracker_req_str;

  HTTP::TCP tracker_server;
  tracker_server.start_connection(TRACKER_URL, 6969);
  int sent_bytes = tracker_server.send_msg(
          tracker_req_str.c_str(), std::strlen(tracker_req_str.c_str()));
      if (sent_bytes < 0)
      {
        std::cerr
            << "[EROR] Message for client list request failed, exiting program";
        fflush(stdout);
        exit(1);
      }

      char resp[65535];
          int recvd = tracker_server.recv_msg(resp, 10000);
          if (recvd < 0)
          {
            std::cerr << "[ERROR] Tracker request failed, exiting program";
            exit(1);
          } else {
              cout << "\n"<<"RECVD Response from tracker server" << std::endl;
          }

          std::string resp_str = resp;
                std::size_t ip_it_start;
                int str_it = 0;


                // Extract Interval

                while (resp_str.find("port", str_it) != std::string::npos)
                {

                  // PARSING IP
                  ip_it_start = resp_str.find("ip", str_it);
                  std::size_t start_ip = resp_str.find(":", ip_it_start);
                  std::size_t end_ip = resp_str.find(":", start_ip + 1);
                  std::string ip = resp_str.substr(start_ip + 1, end_ip - start_ip - 1);

                  // std::cout << "ip: " << ip << std::endl;

                  // PARSING PORT
                  str_it = resp_str.find("port", ip_it_start);
                  std::size_t port_end = resp_str.find("e", str_it);
                  int port =
                      std::stoi(resp_str.substr(str_it + 5, port_end - str_it - 5));
                  // std::cout << "port: " << port << std::endl;
                  str_it = port_end;

                  peers_list[ip] = port;
                }


  return peers_list;
}
