#include <iostream>
#include <sys/socket.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define _XOPEN_SOURCE_EXTENDED 1
#include <regex>
#include <cstring>
#include <netinet/in.h>
#include <netdb.h>

#include "../include/http.h"

using namespace std;

namespace HTTP
{
    
    int TCP::connect_to_peer(std::string ip, int port)
    {
        int peer_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // peer socket


        // initialize a connection
        struct sockaddr_in peer_address;
        memset(&peer_address, 0, sizeof(peer_address));
        peer_address.sin_family = AF_INET;
        peer_address.sin_port = htons(port);
        peer_address.sin_addr.s_addr = inet_addr(ip.c_str());
        int conn = connect(peer_socket, (struct sockaddr*)&peer_address, sizeof(struct sockaddr_in));
        this->to_send_socket = peer_socket;
        cout<< "connectio n: " << conn;
        return conn;

        // Create a socket
        // int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        // if (sockfd < 0) {
        //     std::cerr << "Error: Unable to create socket" << std::endl;
        //     return EXIT_FAILURE;
        // }

        // // Define the server address
        // struct sockaddr_in server_address;
        // memset(&server_address, 0, sizeof(server_address));
        // server_address.sin_family = AF_INET;
        // server_address.sin_port = htons(port);
        // server_address.sin_addr.s_addr = INADDR_ANY;

        // // bind(sockfd, (struct sockaddr*)&server_address, sizeof(server_address));


        // // Convert IP address from string to binary form
        // if (inet_pton(AF_INET, ip.c_str(), &server_address.sin_addr) <= 0) {
        //     std::cerr << "Error: Invalid address or address not supported: " << ip << std::endl;
        //     close(sockfd);
        //     return EXIT_FAILURE;
        // }

        // // Connect to the server
        // if (connect(sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        //     std::cerr << "Error: Connection failed" << std::endl;
        //     close(sockfd);
        //     return EXIT_FAILURE;
        // }

        // std::cout << "Connected to " << ip << ":" << port << std::endl;

        // this->to_send_socket = peer_socket;

        return 0;
    }

    int TCP::start_connection(std::string send_url, int to_send_port)
    {
        to_send_url = send_url;
        to_send_port = to_send_port;
        // Extract host name from send_url
        std::regex regex("(http|https)://([^/]+)");
        std::smatch match;
        if (!std::regex_search(send_url, match, regex))
        {
            std::cerr << "URL is invalid" << std::endl;
            exit(0);
        }
        std::string host = match[2];
        // std::string host = "torrent.ubuntu.com";

        // Creating a socket using TCP
        int server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (server_socket < 0)
        {
            cerr << "[Error] Socket not created on client side";
            exit(0);
        }

        to_send_socket = server_socket;

        // std::cout << "\n socket in create connection " << to_send_socket;

        // Construct the server address
        struct sockaddr_in server_address;
        memset(&server_address, 0, sizeof(server_address));
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(to_send_port); // port number
        struct hostent *server = gethostbyname(host.c_str());
        std::cout << "THE HOST " << host.c_str();
        if (server == NULL)
        {
            std::cerr << "Error resolving hostname" << std::endl;
            return 0;
        }
        memcpy(&server_address.sin_addr.s_addr, server->h_addr, server->h_length);
        // std::cout << "Host addr is:" << server->h_addr;

        // Step 2 : Establish connection with server
        int status = connect(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
        if (status < 0)
        {
            cerr << "[Error] Connection with server not established.\n";
            exit(0);
        }
        else
        {
            return 1;
        }
        return 0;
    }

    int TCP::recv_msg(char *msg, int size_of_msg)
    {
        ssize_t num_of_bytes_recvd = recv(to_send_socket, msg, 1000, 0);
        if (num_of_bytes_recvd < 0)
        {
            std::cout << "\n[Error] recv() failed";
            fflush(stdout);
            exit(0);
        }
        else
        {
            fflush(stdout);
            return 1;
        }
    }

    int TCP::send_msg(const char *msg, int size_of_msg)
    {
        std::cout << "\n sending msg ";
        ssize_t num_of_bytes_recvd = send(to_send_socket, msg, 229, MSG_WAITALL);
        if (num_of_bytes_recvd < 0)
        {
            cerr << "\n[Error] send() failed";
            exit(0);
        }
        else
        {
            return num_of_bytes_recvd;
        }
    }

}
