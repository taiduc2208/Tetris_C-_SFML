// Server.cpp
#include <iostream>
#include <thread>
#include <vector>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <fstream>
#include <sstream>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

const unsigned short SERVER_PORT = 55001;
const unsigned int MAX_CLIENTS = 5;


struct room {
    std::string name;
    std::string passwd;
    bool status;
    int number;
    std::vector<int> sockets;
    int score1 = -1;
    int score2 = -1;
};

std::vector<room> gameRooms;



struct demo
{
    //array declared inside structure
    std::string arr[100];
};
struct account
{
    std::string email;
    std::string password;
};
struct demoAcc
{
    //array declared inside structure
    account arr[100];
};

demoAcc accInfoMain;
int info = 1;

// Hàm để tìm và xóa phòng dựa trên socket
void removeRoomBySocket(int socketToRemove) {
    auto it = std::remove_if(gameRooms.begin(), gameRooms.end(),
        [socketToRemove](const room& r) {
            return std::find(r.sockets.begin(), r.sockets.end(), socketToRemove) != r.sockets.end();
        });
    if (it != gameRooms.end()) {
        gameRooms.erase(it);
    }
}
// Hàm gửi dữ liệu qua socket
bool sendVector2D(SOCKET socket, const std::vector<std::vector<std::uint32_t>>& data) {
    std::size_t rows = 20;
    std::size_t cols = 10;

    // Gửi dữ liệu của mảng
    for (std::size_t i = 0; i < rows; ++i) {
        for (std::size_t j = 0; j < cols; ++j) {
            std::uint32_t value = data[i][j];
            if (send(socket, reinterpret_cast<const char*>(&value), sizeof(value), 0) == -1) {
                return false;
            }
        }
    }

    return true;
}

std::vector<std::vector<std::uint32_t>> recvVector2D(SOCKET socket) {
    std::size_t rows, cols;
    rows = 20; cols = 10;
    std::vector<std::vector<std::uint32_t>> data;

    data.resize(rows);
    for (std::size_t i{}; i < data.size(); ++i) {
        data[i].resize(cols);
    }

    for (std::size_t i = 0; i < rows; ++i) {
        for (std::size_t j = 0; j < cols; ++j) {
            std::uint32_t value;
            if (recv(socket, reinterpret_cast<char*>(&value), sizeof(value), 0) == -1) {
                std::cerr << "Error receiving data element" << std::endl;
                return data;
            }
            data[i][j] = value;
        }
    }

    return data;
}


void sendGameRooms(SOCKET clientSocket, const std::vector<room>& gameRooms) {
    // Gửi số lượng phòng trước
    int numRooms = gameRooms.size();
    send(clientSocket, reinterpret_cast<char*>(&numRooms), sizeof(int), 0);

    // Gửi thông tin từng phòng
    for (const auto& room : gameRooms) {
        // Gửi tên phòng
        int nameSize = room.name.size();
        send(clientSocket, reinterpret_cast<char*>(&nameSize), sizeof(int), 0);
        send(clientSocket, room.name.c_str(), nameSize, 0);

        // Gửi số lượng sockets trong phòng
        int numSockets = room.sockets.size();
        send(clientSocket, reinterpret_cast<char*>(&numSockets), sizeof(int), 0);

        // Gửi thông tin từng socket
        for (int socket : room.sockets) {
            send(clientSocket, reinterpret_cast<char*>(&socket), sizeof(int), 0);
        }
    }
}

struct demoAcc input()
{
    account acc[100];
    info = 1;
    struct demoAcc accInfo;
    std::ifstream fin("clientList.txt", std::ios::in | std::ios::out);
    while (fin >> acc[info].email)
    {
        fin >> acc[info].password;
        accInfo.arr[info] = acc[info];
        //std::cout << acc[info].email << "---" << acc[info].password;
        info++;
    }
    fin.close();
    return accInfo;
}

 struct demo tokenize(std::string s, std::string del = " ")
{
    struct demo result;
    int n = 0;
    int start, end = -1 * del.size();
    do {
        start = end + del.size();
        end = s.find(del, start);
        std::cout << s.substr(start, end - start) << std::endl;
        result.arr[n] = s.substr(start, end - start);
        n++;
    } while (end != -1);
    return result;
}

 bool contains(std::vector<int> vec, int elem)
 {
     bool result = false;
     if (find(vec.begin(), vec.end(), elem) != vec.end())
     {
         result = true;
     }
     return result;
 }

void handleClient(int clientSocket) {
    char buffer[1024];
    int bytesRead;
    std::string messFromClient = "";

    while (true) {
        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            // Error or connection closed
            break;
        }

        // Process received data
        messFromClient = std::string(buffer, bytesRead);
        struct demo clientInfo = tokenize(messFromClient, "||");

        
        if (clientInfo.arr[0] == "LOGIN") {
            std::string e, p;
            e = clientInfo.arr[1];
            p = clientInfo.arr[2];
            int checkSuccess = 0;
            struct demoAcc checkLogin = input();

            for (int j = 1; j <= info; j++)
            {
               // std::cout << checkLogin.arr[j].email << "---" << checkLogin.arr[j].password << std::endl;
                if (e == checkLogin.arr[j].email && p == checkLogin.arr[j].password)
                {
                    send(clientSocket, "+OK||4", 7, 0);
                    checkSuccess++;
                    break;
                }
               
            }
            if(checkSuccess == 0) {
                send(clientSocket, "-NO||5", 7, 0);
            }
        }
        else if (clientInfo.arr[0] == "REGISTER") {
            std::string e, p;
            e = clientInfo.arr[1];
            p = clientInfo.arr[2];
            int checkSuccess = 0;
            struct demoAcc checkLogin = input();

            for (int j = 1; j <= info; j++)
            {
                // std::cout << checkLogin.arr[j].email << "---" << checkLogin.arr[j].password << std::endl;
                if (e == checkLogin.arr[j].email && p == checkLogin.arr[j].password)
                {
                    send(clientSocket, "-NO||5", 7, 0);
                    checkSuccess++;
                    break;
                }

            }
            if (checkSuccess == 0) {
                send(clientSocket, "+OK||5", 7, 0);
                std::ofstream fout("clientList.txt", std::ios::app);
                fout << e << "\n";
                fout << p << "\n";
                fout.close();
            }

        }
        else if (clientInfo.arr[0] == "TRAIN") {
            std::string e, p;
            e = clientInfo.arr[1];
            p = clientInfo.arr[2];
            int checkSuccess = 0;
            std::cout << "Score: " << e << "\n" << std::endl;

        }
        else if (clientInfo.arr[0] == "ADD_ROOM") {
            std::string e, p;
            e = clientInfo.arr[1];
            p = clientInfo.arr[2];
            int checkSuccess = 0;
            // Gửi thông tin từng phòng
            for (const auto& room : gameRooms) {
                if (e == room.name) {
                    send(clientSocket, "-NO||6", 7, 0);
                    checkSuccess++;
                    break;
                }
                
            }
            if (checkSuccess == 0) {
                send(clientSocket, "+OK||7", 7, 0);
                room room1;
                room1.name = e;
                room1.passwd = p;
                room1.number = 1;
                room1.status = false;
                room1.sockets.push_back(clientSocket);  
                gameRooms.push_back(room1);
            }
            

        }
        else if (clientInfo.arr[0] == "JOIN_ROOM") {
            std::string e, p;
            e = clientInfo.arr[1];
            p = clientInfo.arr[2];
            int checkSuccess = 0;
            // Kiểm tra thông tin từng phòng
            for (auto& room : gameRooms) {
                if (e == room.name) {
                    if (p == room.passwd) {
                        
                        if (!contains(room.sockets, clientSocket)) {
                            room.sockets.push_back(clientSocket);
                            room.number = 2;
                            room.status = true;
                            send(clientSocket, "+OK||8", 7, 0);
                        }
                        else {
                            send(clientSocket, "+OK||7", 7, 0);
                        }
                        checkSuccess++;
                        break;
                    }
                    
                }

            }
            if (checkSuccess == 0) {
                send(clientSocket, "-NO||6", 7, 0);

            }


        }
        else if (clientInfo.arr[0] == "LIST") {
            // Gửi số lượng phòng trước
                // Thêm phòng game
            
            int numRooms = gameRooms.size();  // Số lượng phòng (điều này phải được tính toán động trong thực tế)
            if (numRooms > 0) {
                std::cout << numRooms << "\n";
                send(clientSocket, "+OK||6", 7, 0);
                sendGameRooms(clientSocket, gameRooms);
            }
            else {
                send(clientSocket, "-NO||6", 7, 0);
            }
        }
        else if (clientInfo.arr[0] == "TETRIS") {
            send(clientSocket, "-NO||6", 7, 0);
            std::cout << "tetris game send data\n";
        }
        else if (clientInfo.arr[0] == "TETRIS_TEST") {

            std::string e, p;
            e = clientInfo.arr[1];
            p = clientInfo.arr[2];
            int checkSuccess = 0;
            // Kiểm tra thông tin từng phòng
            for (auto& room : gameRooms) {
                if (e == room.name) {
                    for (auto& socket : room.sockets) {
                        if (socket != stoi(p)) {
                            std::cout << socket << " ++ " << p << "\n";
                            send(socket, "+OK||S", 7, 0);
                        }
                    }
                    
                }
            }
        }
        else if (clientInfo.arr[0] == "SCORE") {

            std::string score, roomname;
            score = clientInfo.arr[1];
            roomname = clientInfo.arr[2];
            int checkSuccess = 0;
            // Kiểm tra thông tin từng phòng
            for (auto& room : gameRooms) {
                if (roomname == room.name) {
                    if (room.score1 == -1) {
                        room.score1 = stoi(score);

                        // Xu ly doi thu disconnect
                        //
                    }
                    else {
                        room.score2 = stoi(score);
                        std::cout << "\ncheck score:  " << room.score1 << "++++" << room.score2 << "\n";
                        bool win = (room.score2 > room.score1);
                        bool draw = (room.score2 == room.score1);
                        std::cout << "Server tra ket qua\n" << win << "--" << draw << "\n";
                        for (auto& socket : room.sockets) {
                            std::cout << socket << "--" << clientSocket << "\n";
                            if (socket == clientSocket) {
                                if (draw) send(clientSocket, "+OK||D", 7, 0);
                                else if(win) send(clientSocket, "+OK||W", 7, 0);
                                else send(clientSocket, "+OK||L", 7, 0);
                            }
                            else {
                                if (draw) send(socket, "+OK||D", 7, 0);
                                else if (win) send(socket, "+OK||L", 7, 0);
                                else send(socket, "+OK||W", 7, 0);
                            }
                        }
                    }

                }
            }
        }

        std::cout << "Received from client: " << clientInfo.arr[0] << "\n" << clientSocket << std::endl;
        
        
        // Echo back to the client
        //send(clientSocket, buffer, bytesRead, 0);
    }

    // Close the socket when done

    removeRoomBySocket(clientSocket);
    closesocket(clientSocket);
}

int main() {
    // Initialize Winsock
    room room1;
    room1.name = "Phong1abuabuabu";
    room1.sockets.push_back(12345);  // Thay thế bằng socket thực tế
    room1.sockets.push_back(54321);  // Thêm socket khác nếu cần
    gameRooms.push_back(room1);

    room room2;
    room2.name = "Phong2";
    room2.sockets.push_back(67890);  // Thay thế bằng socket thực tế
    gameRooms.push_back(room2);
    

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock\n";
        return 1;
    }

    // Setup server socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating socket\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error binding socket\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, MAX_CLIENTS) == -1) {
        std::cerr << "Error listening on socket\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::vector<std::thread> clientThreads;

    std::cout << "Server is listening on port " << SERVER_PORT << std::endl;

    while (true) {
        // Accept incoming connections
        sockaddr_in clientAddress;
        socklen_t clientAddressSize = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressSize);

        if (clientSocket != -1) {
            // Handle the client in a separate thread
            clientThreads.emplace_back(handleClient, clientSocket);
        }

        // Remove finished threads
        clientThreads.erase(std::remove_if(clientThreads.begin(), clientThreads.end(),
            [](const std::thread& t) { return !t.joinable(); }),
            clientThreads.end());
    }

    // Close all client sockets
    for (auto& thread : clientThreads) {
        thread.join();
    }

    // Close the server socket
    closesocket(serverSocket);

    // Cleanup Winsock
    WSACleanup();

    return 0;
}