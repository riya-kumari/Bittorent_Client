#include "../include/client.h"
#include <vector>
#include <queue>
#include <http.h>
#include <message.h>
#include <sstream>
#include <string>

using namespace std;

Client::Client(map<string, int> peers_list, string info_hash, string peer_id) : info_hash(info_hash), peer_id(peer_id)
{

    for (const auto &[peer_ip, peer_port] : peers_list)
    {
        this->peers_queue.push(std::make_pair(peer_ip, peer_port));
    }

}


string Client::create_handshake(){
    
    string protocol_identifier_len = "\x13"; // 19 in hex
    string protocol_identifier = "BitTorrent protocol";
    string reserved_bytes = "\x00\x00\x00\x00\x00\x00\x00\x00";
    
    std::ostringstream oss;
    oss << protocol_identifier_len << protocol_identifier << reserved_bytes << this->info_hash << this->peer_id;

    return oss.str();
}
void Client::download_torrent()
{

    size_t MAX_CONECTIONS = 2;

    if (this->peers_queue.size() < MAX_CONECTIONS)
    {
        cerr << "NOT ENOUGH PEERS TO CONNECT TO." << std::endl;
        return;
    }

    cout << "inside download torrent";


    // Download using two peers
    string handshake_msg = this->create_handshake();

    while(this->peers_connected.size() < MAX_CONECTIONS){
        auto peer = peers_queue.front();
        peers_queue.pop();

        HTTP::TCP peer_conn;        
        
        int status = peer_conn.connect_to_peer(peer.first, peer.second);
        if(status != 0) continue;
        fflush(stdout);
        peer_conn.send_msg(handshake_msg.c_str(), strlen(handshake_msg.c_str()));

        char resp[65535];
          int recvd = peer_conn.recv_msg(resp, 10000);
          if (recvd < 0)
          {
            std::cerr << "[ERROR] Peer Request failed";
            continue;
          } else {
              cout << "\n"<<"RECVD Response from tracker server" << std::endl;
          }

    }
}
