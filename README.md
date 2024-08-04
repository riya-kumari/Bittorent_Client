# Bittorent Client
Decentralized peer to peer communication protocol.
A user can share or download files from a peer or multiple of peers in a decentralized way, which offers several advantages, including faster downloads.

### How does it work?

A file is divided into multiple pieces. For instance, if you want to download a Debian ISO file, your client will first request a list of peers from a tracker server. This list includes the IP addresses and ports of peers from whom the client can download. The client then connects to these peers to request the missing pieces. Once all pieces are downloaded, the client can reassemble the file.

Important Terminology:

Leechers: Peers that download data.
Seeders: Peers that upload data.
This project focuses on developing a leecher that downloads pieces from other peers.

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

## Docker Usage

To run the Bittorrent Client using Docker, you can use the following commands:

1. **Run the Docker container:**

   ```bash
   docker pull rkumari1/bittorent:latest
   docker images
   docker run -v ./Bittorrent_Client/:/home -dit -p81:3000 <docker image id>
   docker ps -a
   docker exec -it <container id> /bin/bash -v


