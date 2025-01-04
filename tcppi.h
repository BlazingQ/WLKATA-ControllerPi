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

const int PRE_VRF_TIME = 1000;

struct ServerMsg {
    int armId;
    std::string cmds;
    std::string time;
    int vrfId;
    int vrfRes;
};

void updateLocs(string cmd, float locs[]);
int updateCmds(int vrfid, string cmds[], string durations[], int size, const string newcmds);
string timenow();
void sleeppi(int duration);
string statusWrapper(int armid, int cmdid, int isinit, string statuss[], int statuss_len);
string updateStatusJson(int armid, int cmdid, string cmds[], string durations[], int size, float locs[], string starttime);
ServerMsg parseServerMsg(const std::string& jsonString);
int getVrfTime(int cmdid, string starttime, int vrfid, string durations[]);

std::vector<std::string> decodeCommaStr(const std::string& commands, int startIndex, int length = INT_MAX);
std::string subCommaStr(const std::string& commands, int startIndex, int length);
std::string encodeCommaStr(std::string commandArray[], int arraysize, int startIndex, int length = INT_MAX);
vector<int> commaStrtoInt(const string& commands, int startIndex, int length = INT_MAX);
void sendmsgThread(Mirobot* ptr, string str, int* flag);
void appendToFile(const std::string& str, const std::string& filename);
void overwriteToFile(const string& str, const string& filename);
int create_connection(const char* server_ip, int server_port);
bool send_message(int sock, const char* message);
bool receive_message(int sock, char* buffer, size_t buffer_size);
void close_connection(int sock);


#endif