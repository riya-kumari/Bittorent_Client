#ifndef TRACKER_SERVER_H
#define TRACKER_SERVER_H

#include <iostream>
#include <map>


struct TrackerRequest
{
  std::string info_hash;
  std::string peer_id;
  int port;
  int uploaded;
  int downloaded;
  int left;
  int compact;
  int no_peer_id;
  std::string events; // started, stopped, completed
  int numwant;        // optional
  std::string trackerid;      // optional
  int key;            // optional
};

class TrackerServer {
    public:
        TrackerRequest req;
        std::string tracker_req_str;
        std::string info_hash;
        std::string peer_id;

        TrackerServer(std::string info_hash) : info_hash(info_hash){};
        std::map<std::string, int> create_tracker_req();
        std::string generate_peer_id();

};

#endif // TRACKER_SERVER_H
