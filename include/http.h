#ifndef HTTP_H
#define HTTP_H

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


namespace HTTP{
    class TCP{
        public:
            std::string my_ip;
            int my_port;
            int my_socket;
            std::string to_send_url;
            struct sockaddr_in to_send_addr;
            int sockAddrLen;
            int to_send_socket;
            int to_send_port;

            int start_connection(std::string send_url, int port);
            int recv_msg(char* msg, int size_of_msg);
            int send_msg(const char* msg, int size_of_msg);
    };
}

#endif // HTTP_H
