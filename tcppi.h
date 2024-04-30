#ifndef TCPP_I_H
#define TCPP_I_H

#include "mirobot.h"

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <thread>
#include <fstream>
using namespace std;

void updateLocs(string cmd, float locs[]);
void sendmsgThread(Mirobot* ptr, string str, int* flag);
void appendToFile(const std::string& str, const std::string& filename);
int create_connection(const char* server_ip, int server_port);
bool send_message(int sock, const char* message);
bool receive_message(int sock, char* buffer, size_t buffer_size);
void close_connection(int sock);

// void listen(const char* dstIP, int dstPort);
// void send(const char* dstIP, int dstPort, char* message);

#endif