#include "tcppi.h"


void sendmsgThread(Mirobot* ptr, string str, int* flag){
    if(ptr){
        ptr->send_msg(str, true);
    }
    *flag = 1;
    
    // cout<<"subthread change syn through ptr\n";
    // cout<<endl<<"*flag = "<< *flag<<" flag addr = "<<flag<<endl;
}

string timenow(){
    auto now = std::chrono::high_resolution_clock::now();
    auto duration_since_epoch = now.time_since_epoch();
    long long int millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration_since_epoch).count();
    return to_string(millis);
}

cJSON* statusToJsonstr(string status[]) {
    // 创建一个新的 cJSON 对象
    cJSON *json = cJSON_CreateObject();

    // 添加数据到 JSON 对象，注意处理整数转换
    cJSON_AddNumberToObject(json, "ArmId", stoi(status[0]));
    cJSON_AddNumberToObject(json, "CmdId", stoi(status[1]));
    cJSON_AddStringToObject(json, "Cmds", status[2].c_str());
    cJSON_AddStringToObject(json, "Locs", status[3].c_str());
    cJSON_AddStringToObject(json, "LastCmdTime", status[4].c_str());
    
    return json;
}

void updateLocs(string cmd, float locs[]){
    istringstream stream(cmd);
    string token;
    char identifier;
    float value;
    bool isincre = false;
    while (stream >> token) {
        if (token[0] == 'X' || token[0] == 'Y' || token[0] == 'Z' || 
            token[0] == 'A' || token[0] == 'B' || token[0] == 'C') {
            identifier = token[0];
            size_t idx = 1; // 开始解析的位置，跳过标识符

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

void appendToFile(const std::string& str, const std::string& filename) {
    // 打开文件，使用 std::ios::app 模式以追加方式写入
    std::ofstream file(filename, std::ios::app);
    if (file.is_open()) {
        // 写入字符串并添加换行符
        file << str << ',';
        // 关闭文件
        file.close();
    } else {
        // 文件打开失败，输出错误消息
        std::cerr << "Unable to open file: " << filename << std::endl;
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
