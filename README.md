# Bittorent Client
Decentralized peer to peer communication protocol.
A user can share or download files from a peer or multiple of peers in a decentralized way, which offers several advantages, including faster downloads.

### How does it work?

A single file is broken down into multiple _pieces_. For downloading a file, the current user would make a request to the tracker server, which will return a list of peers, containing each peer's ip and port. The current user client will then establish connections with peers from the list, requesting the missing pieces. Once it has all the pieces it can assemble a file. 

Important terminology : 
Leechers : Peers that download data. 
Seeders  : Peers that upload data.

This project is focused on creating a leecher that downloads from other peers.

Current progress (this has been tested and is working) : 
- Parse a torrent file
- Make tracker server request and obtain a list of peers
*Both HTTP and HTTPS tracker servers are supported
- Create connections to peers

What needs to be done:
- Message passing
- Begin download

## Installation

1. **Setup build environment:**

Can use docker : 
```
docker pull rkumari1/bittorent:latest
```

Prerequisites
- C++17 or later
- CMake 3.10 or later
- Boost libraries
- OpenSSL

2. **Clone the repository:**
   ```bash
   git clone https://github.com/yourusername/Bittorrent_Client.git
   cd Bittorrent_Client
   make

3. **Run it**
```
./bittorent -f <torrent filepath>
```

