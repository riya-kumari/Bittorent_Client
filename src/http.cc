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

namespace HTTP{

                int TCP::start_connection(std::string send_url, int to_send_port){
                        to_send_url = send_url;
                        to_send_port = to_send_port;
                        // Extract host name from send_url
                            std::regex regex("(http|https)://([^/]+)");
                            std::smatch match;
                            if (!std::regex_search(send_url, match, regex)) {
                                std::cerr << "URL is invalid" << std::endl;
                                exit(0);
                            }
                            std::string host = match[2];

                        // Creating a socket using TCP
                        int server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                        if (server_socket < 0) {
                            cerr << "[Error] Socket not created on client side";
                            exit(0);
                        }

                        to_send_socket = server_socket;

                        std::cout << "\n socket in create connection " << to_send_socket;

                        // Construct the server address
                        struct sockaddr_in server_address;
                        memset(&server_address, 0, sizeof(server_address));
                        server_address.sin_family = AF_INET;
                        server_address.sin_port = htons(to_send_port); // port number
                        struct hostent *server = gethostbyname(host.c_str());
                        std::cout << "THE HOST " << host.c_str() ;
                            if (server == NULL) {
                                std::cerr << "Error resolving hostname" << std::endl;
                                return 0;
                            }
                        memcpy(&server_address.sin_addr.s_addr, server->h_addr, server->h_length);
                        //std::cout << "Host addr is:" << server->h_addr;

                        // Step 2 : Establish connection with server
                        int status = connect(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));
                        if(status < 0){
                            cerr << "[Error] Connection with server not established.\n";
                            exit(0);
                        } else {
                            return 1;
                        }
                        return 0;

                }

                int TCP::recv_msg(char* msg, int size_of_msg){

                    // std::cout << "\nIn receive from "<< to_send_socket;
                    // fflush(stdout);
                   	ssize_t num_of_bytes_recvd = recv(to_send_socket, msg, 1000, 0);
                   	if(num_of_bytes_recvd < 0){
                  		std::cout << "\n[Error] recv() failed";
                        fflush(stdout);
                  		exit(0);
                   	} else {
                        // std::cout << "msg recvd : " << msg;
                        fflush(stdout);
                        return 1;
                    }

                }
                int TCP::send_msg(const char* msg, int size_of_msg){
                // int TCP::send_msg(std::string msg, int size_of_msg){
                    //std::cout << "\n sending this msg " << msg ;
                    std::cout << "\n sending msg " ;
                    ssize_t num_of_bytes_recvd = send(to_send_socket, msg, 229, MSG_WAITALL);
                   	if(num_of_bytes_recvd < 0){
                  		cerr << "\n[Error] send() failed";
                  		exit(0);
                   	} else {
                        return num_of_bytes_recvd;
                    }
                }


}
