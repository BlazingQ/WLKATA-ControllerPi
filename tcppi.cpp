#include "tcppi.h"


void sendmsgThread(Mirobot* ptr, string str, int* flag){
    if(ptr){
        ptr->send_msg(str, true);
    }
    *flag = 1;
    
}

//应该有必要传递一下谁需要验证，跳过了空字符串
string statusWrapper(int armid, int vrfid, int isinit, string statuss[], int statuss_len) {
    cJSON *json = cJSON_CreateObject();

    cJSON_AddNumberToObject(json, "ArmId", armid);
    cJSON_AddNumberToObject(json, "VrfId", vrfid);
    cJSON_AddNumberToObject(json, "IsInit", isinit);
    cJSON_AddStringToObject(json, "ConstructTime", timenow().c_str());
    cJSON *armsArray = cJSON_CreateArray();
    // cJSON *finalArray = cJSON_CreateArray();
    for (int i = 0; i < statuss_len; i++) {
        if (statuss[i].empty()) {
            continue;  // 跳过空字符串
        }
        cJSON *statusItem = cJSON_Parse(statuss[i].c_str());
        if (statusItem != NULL) {
            cJSON_AddItemToArray(armsArray, statusItem);
        } else {
            // 如果解析失败，释放已创建的 JSON 对象并返回 NULL
            cJSON_Delete(json);
            return NULL;
        }
    }

    cJSON_AddItemToObject(json, "Arms", armsArray);

    return string(cJSON_Print(json));
}

//根据ARM存储的内容更新自身状态，传输给中控. cmdid对应cmds中第一条指令的index
string updateStatusJson(int armid, int cmdid, string cmds[], string durations[], int size, float locs[], string starttime) {

    cJSON *json = cJSON_CreateObject();
    std::ostringstream oss1;
    for(int i = 0; i < 6; ++i) {
        if(i != 0) {
            oss1 << ","; 
        }
        oss1 << locs[i];
    }
    string locsstr = oss1.str();

    std::ostringstream oss2;
    std::ostringstream oss3;
    for(int i = 0; i < 10; ++i) {
        if(i + cmdid < size){
            if(i != 0) {
                oss2 << ","; 
                oss3 << ",";
            }
            oss2 << cmds[i + cmdid];
            oss3 << durations[i + cmdid];
        }
    }
    string newcmds = oss2.str();
    string newdurations = oss3.str();

    
    cJSON_AddNumberToObject(json, "ArmId", armid);
    cJSON_AddNumberToObject(json, "CmdId", cmdid);
    cJSON_AddStringToObject(json, "Cmds", newcmds.c_str());
    cJSON_AddStringToObject(json, "Durations", newdurations.c_str());
    cJSON_AddStringToObject(json, "Locs", locsstr.c_str());
    cJSON_AddStringToObject(json, "StartTime", starttime.c_str());

    string jsonstr = string(cJSON_Print(json));
    
    return jsonstr;
}

ServerMsg parseServerMsg(const std::string& jsonString) {
    ServerMsg result = {0, "", "", 0, 0};

    // 解析 JSON 字符串
    cJSON* json = cJSON_Parse(jsonString.c_str());
    if (json == nullptr) {
        std::cerr << "JSON解析失败" << std::endl;
        return result;
    }

    // 提取字段并存储在结构体中
    cJSON* armid = cJSON_GetObjectItem(json, "ArmId");
    cJSON* cmds = cJSON_GetObjectItem(json, "Cmds");
    cJSON* time = cJSON_GetObjectItem(json, "Time");
    cJSON* vrfid = cJSON_GetObjectItem(json, "VrfId");
    cJSON* vrfres = cJSON_GetObjectItem(json, "VrfRes");

    if (armid != nullptr && cmds != nullptr && time != nullptr && vrfid != nullptr && vrfres != nullptr) {
        if (cJSON_IsNumber(armid) && cJSON_IsString(cmds) && cJSON_IsString(time) && 
            cJSON_IsNumber(vrfid) && cJSON_IsNumber(vrfres)) { //type dismatch leading to segmentation fault
            result.armId = armid->valueint;
            result.cmds = cmds->valuestring;
            result.time = time->valuestring;
            result.vrfId = vrfid->valueint;
            result.vrfRes = vrfres->valueint;
        }else{
            std::cerr << "JSON type dismatch" << std::endl;
        }
    }

    cJSON_Delete(json);
    return result;
}

/*参数：vrfid 被验证的cmd的index并需要将其替换，newcmds 返回的服务器消息中的用于替换的cmds
返回：修改后的总的cmds和durations的size大小，在YYAUTO中更新*/
int updateCmds(int vrfid, string cmds[], string durations[], int size, const string newcmds) {
    vector<string> newCmdArray = decodeCommaStr(newcmds, 0, INT_MAX);
    int newSize = size - 1 + newCmdArray.size();
    
    // Shift existing commands after vrfid to make space
    for(int i = size-1; i > vrfid; i--) {
        if(newCmdArray.size() > 1) {
            cmds[i + newCmdArray.size() - 1] = cmds[i];
            durations[i + newCmdArray.size() - 1] = durations[i];
        }
    }
    
    // Insert new commands
    for(int i = 0; i < newCmdArray.size(); i++) {
        cmds[vrfid + i] = newCmdArray[i];
        if(i > 0){
            durations[vrfid + i] = "1000";
        }
    }
    
    return newSize;
}

//这个算法的正确基于发出的指令被执行，即指令本身不能是错的
void updateLocs(string cmd, float locs[]){
    istringstream stream(cmd);
    string token;
    char identifier;
    float value;
    bool isincre = false; //一次只考虑当前这一条指令，简单这么写就行
    while (stream >> token) {
        if (token[0] == 'X' || token[0] == 'Y' || token[0] == 'Z' || 
            token[0] == 'A' || token[0] == 'B' || token[0] == 'C') {
            identifier = token[0];
            int idx = 1; // 开始解析的位置，跳过标识符

            // 处理数值可能直接跟在标识符后面的情况
            if (!isdigit(token[idx]) && token[idx] != '-' && token[idx] != '+') {
                idx++; // 如果标识符后面有空格，则跳过
            }

            value = stof(token.substr(idx));

            if(!isincre){
                if (identifier == 'X') {
                    locs[0] = value;
                } else if (identifier == 'Y') {
                    locs[1] = value;
                } else if (identifier == 'Z') {
                    locs[2] = value;
                } else if (identifier == 'A') {
                    locs[3] = value;
                } else if (identifier == 'B') {
                    locs[4] = value;
                } else if (identifier == 'C') {
                    locs[5] = value;
                }
            }else{
                if (identifier == 'X') {
                    locs[0] += value;
                } else if (identifier == 'Y') {
                    locs[1] += value;
                } else if (identifier == 'Z') {
                    locs[2] += value;
                } else if (identifier == 'A') {
                    locs[3] += value;
                } else if (identifier == 'B') {
                    locs[4] += value;
                } else if (identifier == 'C') {
                    locs[5] += value;
                }
            }
        }else if(token == "G91"){
            isincre = true;
        }
    }
}


//直接假定两个id都是完全有效的，还需要保证starttime和cmdid完全对应
int getVrfTime(int cmdid, string starttime, int vrfid, string durations[]){
    int duration = 0;
    int ret = 0;
    for(int i = cmdid; i < vrfid; ++i){
        duration += stoi(durations[i]);
    }
    int temptime = static_cast<int>(stoll(timenow()) - stoll(starttime));
    ret = max(0, duration - temptime - PRE_VRF_TIME);
    return ret;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
}

// 将单个指令串转换为字符串数组，指定起始索引和长度
std::vector<std::string> decodeCommaStr(const std::string& commands, int startIndex, int length) {
    std::vector<std::string> commandArray;
    std::stringstream ss(commands);
    std::string command;
    int index = 0;

    while (std::getline(ss, command, ',')) {
        if (index >= startIndex && index < startIndex + length) {
            commandArray.push_back(command);
        }
        index++;
    }

    return commandArray;
}

std::string subCommaStr(const std::string& commands, int startIndex, int length) {
    std::string subcmdstr;
    std::stringstream ss(commands);
    std::string command;
    int index = 0;

    while (std::getline(ss, command, ',')) {
        if (index >= startIndex && index < startIndex + length) {
            if(!subcmdstr.empty()){
                subcmdstr += ",";
            }
            subcmdstr += command;
        }
        index++;
    }

    return subcmdstr;
}

// 将字符串数组转换为单个指令串，指定起始索引和长度
std::string encodeCommaStr(std::string commandArray[], int arraysize, int startIndex, int length) {
    std::string commands;
    
    for (int i = startIndex; i < startIndex + length && i < arraysize; ++i) {
        commands += commandArray[i];
        if (i < startIndex + length - 1 && i < arraysize - 1) {
            commands += ","; // 在每个指令后添加逗号，除了最后一个
        }
    }

    return commands;
}

vector<int> commaStrtoInt(const string& commands, int startIndex, int length) {
    vector<int> commandArray;
    stringstream ss(commands);
    string command;
    int index = 0;

    while (getline(ss, command, ',')) {
        if (index >= startIndex && index < startIndex + length) {
            commandArray.push_back(stoi(command));
        }
        index++;
    }

    return commandArray;
}

string timenow(){
    auto now = std::chrono::high_resolution_clock::now();
    auto duration_since_epoch = now.time_since_epoch();
    long long int millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration_since_epoch).count();
    return to_string(millis);
}

void sleeppi(int duration){
    std::this_thread::sleep_for(std::chrono::milliseconds(duration));
}


void appendToFile(const std::string& str, const std::string& filename) {
    // 打开文件，使用 std::ios::app 模式以追加方式写入
    std::ofstream file(filename, std::ios::app);
    if (file.is_open()) {
        file << str;
        // 关闭文件
        file.close();
    } else {
        // 文件打开失败，输出错误消息
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
}

void overwriteToFile(const string& str, const string& filename) {
    // 打开文件，使用 ios::out 模式以覆盖方式写入
    ofstream file(filename, ios::out);
    if (file.is_open()) {
        // 写入字符串并添加换行符
        file << str;
        // 关闭文件
        file.close();
    } else {
        // 文件打开失败，输出错误消息
        cerr << "Unable to open file: " << filename << endl;
    }
}

int create_connection(const char* server_ip, int server_port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported." << std::endl;
        close(sock);
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection Failed." << std::endl;
        close(sock);
        return -1;
    }

    return sock;
}

bool send_message(int sock, const char* message) {
    if (send(sock, message, strlen(message), 0) < 0) {
        std::cerr << "Failed to send message." << std::endl;
        return false;
    }
    std::cout << "Message sent successfully." << std::endl;
    return true;
}

bool receive_message(int sock, char* buffer, size_t buffer_size) {
    ssize_t bytes_received = recv(sock, buffer, buffer_size, 0);
    if (bytes_received < 0) {
        std::cerr << "Error in receiving data." << std::endl;
        return false;
    } else if (bytes_received == 0) {
        std::cerr << "Connection closed by the peer." << std::endl;
        return false;
    }
    buffer[bytes_received] = '\0'; // Ensure null-termination
    std::cout << "Received: " << buffer << std::endl;
    return true;
}

void close_connection(int sock) {
    close(sock);
    std::cout << "Connection closed." << std::endl;
}
