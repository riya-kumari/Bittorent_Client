#include <vector>
#include <map>
#include <queue>
#include <string>


using namespace std;

class Client {
    private : 
        queue<pair<string, int>> peers_queue;
        vector<pair<string, int>> peers_connected;
        string info_hash;
        string peer_id;
        string create_handshake();
    public:
        Client(map<string, int> peers_list, string info_hash, string peer_id);
        void download_torrent();
        

};