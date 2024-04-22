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
using namespace std;

void sendmsgThread(Mirobot* ptr, string str, int* flag);
int create_connection(const char* server_ip, int server_port);
bool send_message(int sock, char* message);
bool receive_message(int sock, char* buffer, size_t buffer_size);
void close_connection(int sock);

// void listen(const char* dstIP, int dstPort);
// void send(const char* dstIP, int dstPort, char* message);

#endif