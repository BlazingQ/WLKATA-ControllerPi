#ifndef TCPP_I_H
#define TCPP_I_H

#include "mirobot.h"
// #include "json.hpp"
#include "cJSON.h"

#include <iostream>
#include<unordered_map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <thread>
#include <fstream>
#include <chrono>
#include <climits>
using namespace std;
// using json = nlohmann::json;

const int leadtime = 2000;

void updateLocs(string cmd, float locs[]);
string timenow();
void sleeppi(int duration);
string statusWrapper(int armid, int cmdid, int isinit, string statuss[], int statuss_len);
string updateStatusJson(int armid, int cmdid, string cmds, string durations, float locs[], string starttime);
std::unordered_map<std::string, int> parseServerMsg(const std::string& jsonString);
int getVrfTime(int cmdid, string starttime, int vrfid, string durations[]);

std::vector<std::string> decodeCommaStr(const std::string& commands, int startIndex, int length);
std::string subCommaStr(const std::string& commands, int startIndex, int length);
std::string encodeCommaStr(std::string commandArray[], int arraysize, int startIndex, int length);
vector<int> commaStrtoInt(const string& commands, int startIndex, int length);
void sendmsgThread(Mirobot* ptr, string str, int* flag);
void appendToFile(const std::string& str, const std::string& filename);
void overwriteToFile(const string& str, const string& filename);
int create_connection(const char* server_ip, int server_port);
bool send_message(int sock, const char* message);
bool receive_message(int sock, char* buffer, size_t buffer_size);
void close_connection(int sock);


#endif