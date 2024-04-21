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
void listen(const char* dstIP, int dstPort);
void send(const char* dstIP, int dstPort, char* message);

#endif