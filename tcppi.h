#ifndef TCPP_I_H
#define TCPP_I_H

#include "mirobot.h"
// #include "json.hpp"
#include "cJSON.h"

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <thread>
#include <fstream>
using namespace std;
// using json = nlohmann::json;

void updateLocs(string cmd, float locs[]);
string timenow();
string statusWrapper(int armid, string statuss[], int statuss_len);
string updateStatusJson(int armid, int cmdid, string cmds, string durations, float locs[]);
void sendmsgThread(Mirobot* ptr, string str, int* flag);
void appendToFile(const std::string& str, const std::string& filename);
int create_connection(const char* server_ip, int server_port);
bool send_message(int sock, const char* message);
bool receive_message(int sock, char* buffer, size_t buffer_size);
void close_connection(int sock);


#endif