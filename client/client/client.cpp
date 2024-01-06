// Client.cpp
#include <SFML/Graphics.hpp>

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <fstream>
#include "textbox.h"
#include <time.h>
#include "tetris.h"
#include "tetris2.h"
#include <sstream>
#include "room.h"
#include "alert.h"
#include <vector>
#include "addRoom.h"
#include "formAddRoom.h"
#include <thread>
#include <future>
#include <algorithm>
#include <string.h>
#include "find.h"



#pragma comment(lib, "ws2_32.lib")


const char* SERVER_IP = "127.0.0.1";
const unsigned short SERVER_PORT = 55001;
const unsigned short CLIENT_SERVER_PORT = 55005;
const int BUFFER_SIZE = 1024;


bool typing = true;
using namespace sf;


struct scn2
{
    bool email = false;
    bool password = false;
};

struct scn3
{
    bool email = false;
    bool confirmEmail = false;
    bool password = false;
    bool confirmPassword = false;
};

struct scnGame
{
	bool train = false;
	bool play = false;
	bool mypage = false;
	bool ranking = false;
};

struct account
{
    std::string email;
    std::string password;
};

int scene = 1, i = 1;
scn2 scene2;
scn3 scene3;
scnGame sceneGame;
account acc[100];
bool first = true;
bool updateListRoom = false;
bool hasLogin = false;
std::string nameLogin;

struct GameRoom {
	std::string name;
	int number;
	bool status;
	std::vector<int> sockets;
};

GameRoom myRoom;

struct Player {
	std::string name;
	std::string score;
};

std::vector<GameRoom> receiveGameRooms(SOCKET serverSocket) {
	std::vector<GameRoom> receivedGameRooms;

	// Nhận số lượng phòng
	int numRooms;
	recv(serverSocket, reinterpret_cast<char*>(&numRooms), sizeof(int), 0);
	std::cout << numRooms << "\n";
	// Nhận thông tin từng phòng
	for (int i = 0; i < numRooms; ++i) {
		GameRoom room;

		// Nhận tên phòng
		int nameSize;
		recv(serverSocket, reinterpret_cast<char*>(&nameSize), sizeof(int), 0);
		char nameBuffer[256];  // Điều này là giả định, bạn có thể sử dụng động để tránh giới hạn kích thước cứng
		recv(serverSocket, nameBuffer, nameSize, 0);
		room.name = std::string(nameBuffer, nameSize);
		//std::cout << room.name << "\n";
		// Nhận số lượng sockets trong phòng
		int numSockets;
		recv(serverSocket, reinterpret_cast<char*>(&numSockets), sizeof(int), 0);
		room.number = numSockets;
		
		// Nhận thông tin từng socket
		for (int j = 0; j < numSockets; ++j) {
			int socket;
			recv(serverSocket, reinterpret_cast<char*>(&socket), sizeof(int), 0);
			room.sockets.push_back(socket);
		}
		if (numSockets > 1) {
			room.status = true;
		}
		else room.status = false;
		// Thêm thông tin phòng vào vector
		receivedGameRooms.push_back(room);
	}

	return receivedGameRooms;
}

std::vector<Player> receiveRank(SOCKET serverSocket) {
	std::vector<Player> receivedRank;

	// Nhận số lượng phòng
	int numRooms123;
	recv(serverSocket, reinterpret_cast<char*>(&numRooms123), sizeof(int), 0);
	std::cout << numRooms123 << "\n";
	// Nhận thông tin từng phòng
	for (int i = 0; i < numRooms123; ++i) {
		Player player;

		// Nhận tên acc
		int nameSize;
		recv(serverSocket, reinterpret_cast<char*>(&nameSize), sizeof(int), 0);
		char nameBuffer123[256];  // Điều này là giả định, bạn có thể sử dụng động để tránh giới hạn kích thước cứng
		recv(serverSocket, nameBuffer123, nameSize, 0);
		player.name = std::string(nameBuffer123, nameSize);
		std::cout << "\n" << player.name << " --- ";
		

		// Nhận score
		int scoreSize;
		recv(serverSocket, reinterpret_cast<char*>(&scoreSize), sizeof(int), 0);
		char nameBuffer321[256];  // Điều này là giả định, bạn có thể sử dụng động để tránh giới hạn kích thước cứng
		recv(serverSocket, nameBuffer321, scoreSize, 0);
		player.score = std::string(nameBuffer321, scoreSize);
		std::cout  << player.score << " \n ";


		// Thêm thông tin phòng vào vector
		receivedRank.push_back(player);
	}

	return receivedRank;
}

struct demo
{
	//array declared inside structure
	std::string arr[100];
};

struct demo tokenize(std::string s, std::string del = " ")
{
	struct demo result;
	int n = 0;
	int start, end = -1 * del.size();
	do {
		start = end + del.size();
		end = s.find(del, start);
		result.arr[n] = s.substr(start, end - start);
		n++;
	} while (end != -1);
	return result;
}
void handleClient(SOCKET clientSocket) {
	char buffer[BUFFER_SIZE];
	recv(clientSocket, buffer, BUFFER_SIZE, 0);
	std::cout << "Received data from client: " << buffer << std::endl;
	closesocket(clientSocket);
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




SOCKET setupServer(int port) {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "Failed to initialize Winsock." << std::endl;
		exit(EXIT_FAILURE);
	}

	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == INVALID_SOCKET) {
		std::cerr << "Error creating server socket." << std::endl;
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		std::cerr << "Error binding server socket. Error code: " << WSAGetLastError() << std::endl;
		closesocket(serverSocket);
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
		std::cerr << "Error listening on server socket. Error code: " << WSAGetLastError() << std::endl;
		closesocket(serverSocket);
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	std::cout << "Server is listening on port " << port << "." << std::endl;

	return serverSocket;
}
SOCKET connectToServer(const char* serverIP, int serverPort, const char* serverName) {
	// Initialize Winsock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "Failed to initialize Winsock." << std::endl;
		exit(EXIT_FAILURE);
	}

	// Setup client socket
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "Error creating client socket for " << serverName << "." << std::endl;
		WSACleanup();
		exit(EXIT_FAILURE);
	}



	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(serverPort);
	inet_pton(AF_INET, serverIP, &serverAddr.sin_addr);
	// Connect to the server
	if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		std::cerr << "Error connecting to " << serverName << ". Error code: " << WSAGetLastError() << std::endl;
		closesocket(clientSocket);
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	std::cout << "Connected to " << serverName << "." << std::endl;

	return clientSocket;
}


int main() {

	myRoom.name = "";
	std::string serverAdd = "127.0.0.1";
	SOCKET clientSocket = connectToServer(serverAdd.c_str(), SERVER_PORT, "Server1");

	

	
	std::vector<Room> buttonList;
	std::vector<Room> textRank;

   
	RenderWindow window(VideoMode(600, 600), "Login System made by Abu");
	Texture t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12;

	t1.loadFromFile("images/Login.png");
	t2.loadFromFile("images/Register.png");
	t3.loadFromFile("images/Email.png");
	t4.loadFromFile("images/Password.png");
	t5.loadFromFile("images/Submit.png");
	t6.loadFromFile("images/Back.png");
	t7.loadFromFile("images/ConfirmEmail.png");
	t8.loadFromFile("images/ConfirmPassword.png");
	t9.loadFromFile("images/LoginOk.png");
	t10.loadFromFile("images/loginFail.png");
	t11.loadFromFile("images/play.png");
	t12.loadFromFile("images/ready.png");

	Sprite login(t1);
	Sprite registerr(t2);
	Sprite Email(t3);
	Sprite Password(t4);
	Sprite Submit(t5);
	Sprite Back(t6);
	Sprite ConfirmEmail(t7);
	Sprite ConfirmPassword(t8);
	Sprite LoginOK(t9);
	Sprite loginFail(t10);
	Sprite playButton(t11);
	Sprite readyButton(t12);

	Font arial;
	arial.loadFromFile("font/arial.ttf");

	
	Textbox textLoginEmail(30, Color::White, false);
	textLoginEmail.setFont(arial);
	
	Textbox textLoginPassword(30, Color::White, false);
	textLoginPassword.setFont(arial);
	Textbox textRegisterEmail(30, Color::White, false);
	textRegisterEmail.setFont(arial);
	Textbox textRegisterConfirmEmail(30, Color::White, false);
	textRegisterConfirmEmail.setFont(arial);
	Textbox textRegisterPassword(30, Color::White, false);
	textRegisterPassword.setFont(arial);
	Textbox textRegisterConfirmPassword(30, Color::White, false);
	textRegisterConfirmPassword.setFont(arial);


	// Tạo các đối tượng hình ảnh
	sf::Texture logoTexture;
	logoTexture.loadFromFile("images/logo.jpg");
	sf::Sprite logoSprite(logoTexture);
	// Tạo các đối tượng nút
	sf::RectangleShape buttonTrain(sf::Vector2f(150, 80));
	sf::RectangleShape buttonFight(sf::Vector2f(150, 80));
	sf::RectangleShape buttonTest(sf::Vector2f(150, 80));
	sf::Text textButtonTrain, textButtonFight, textButtonTest;


	// Tạo các đối tượng nút nhỏ
	sf::CircleShape smallButton1(30.f);
	sf::CircleShape smallButton2(30.f);


	sf::RectangleShape backButton(sf::Vector2f(100, 50));
	backButton.setFillColor(sf::Color(100, 100, 100));

	sf::Text backText;
	backText.setFont(arial);
	backText.setCharacterSize(20);
	backText.setFillColor(sf::Color::Black);
	backText.setString("Back");


	sf::RectangleShape logOutButton(sf::Vector2f(150, 80));

	sf::Text logOutText;
	logOutText.setFont(arial);
	logOutText.setCharacterSize(20);
	logOutText.setFillColor(sf::Color::Black);
	logOutText.setString("Logout");


	sf::RectangleShape searchButton(sf::Vector2f(150, 40));
	sf::Text searchText;
	searchText.setFont(arial);
	searchText.setCharacterSize(20);
	searchText.setFillColor(sf::Color::Black);
	searchText.setString("Search");

	

	AddRoom addRoom("+", arial, 60, sf::Vector2f(500.0f, 50.0f));

    while (window.isOpen()) {
        
		
			Event e;
			while (window.pollEvent(e))
			{
				if (e.type == Event::Closed)
				{
					// send to server
					// Send data to the server
					if (hasLogin) {
						std::string inter = "LOGOUT";
						std::string message = inter + "||" + nameLogin + "|| -";
						send(clientSocket, message.c_str(), message.size(), 0);

					}
					window.close();
				}
				if (e.type == Event::MouseButtonPressed)
				{
					if (e.key.code == Mouse::Left)
					{
						int x = Mouse::getPosition(window).x;
						int y = Mouse::getPosition(window).y;
						if (scene == 1)
						{
							if (x >= 75 && x <= 525 && y >= 87 && y <= 257)
							{
								// LOGIN
								scene = 2;

							}
							if (x >= 75 && x <= 525 && y >= 343 && y <= 513)
							{
								// REGISTER
								scene = 3;
							}
						}
						if (scene == 2)
						{
							// back
							if (x >= 233 && x <= 368 && y >= 530 && y <= 580)
							{
								scene = 1;
							}
							if (x >= 50 && x <= 200 && y >= 75 && y <= 125)
							{
								// click on email
								scene2.email = true;
								textLoginEmail.setSelected(true);
							}
							if (x >= 50 && x <= 259 && y >= 235 && y <= 285)
							{
								//click on password
								scene2.password = true;
								textLoginPassword.setSelected(true);
							}
							// submit
							if (x >= 143 && x <= 457 && y >= 405 && y <= 480)
							{

								std::string e, p;
								e = textLoginEmail.getText();
								p = textLoginPassword.getText();
								if (e.empty() == false && p.empty() == false)
								{
									// send to server
									// Send data to the server
									std::string inter = "LOGIN";
									std::string message = inter+"||" + e + "||" + p;
									send(clientSocket, message.c_str(), message.size(), 0);

									// Receive data from the server
									char buffer[1024];
									int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
									if (bytesRead > 0) {
										buffer[bytesRead] = '\0';
										std::cout << "Received from server: " << buffer << std::endl;
										std::string messFromServer = std::string(buffer, bytesRead);
										struct demo messInfo = tokenize(messFromServer, "||");
										if (messInfo.arr[0] == "+OK") {
											nameLogin = e;
											hasLogin = true;
											scene = 4;
										}
										else if (messInfo.arr[0] == "-NO") {
											scene = 5;
										}
									}
									
								}
							
							}
						}
						if (scene == 3)
						{
							// back
							if (x >= 233 && x <= 368 && y >= 530 && y <= 580)
							{
								scene = 1;
							}
							// email
							if (x >= 15 && x <= 165 && y >= 34 && y <= 84)
							{
								scene3.email = true;
								textRegisterEmail.setSelected(true);
							}
							// confirm email
							if (x >= 15 && x <= 293 && y >= 116 && y <= 166)
							{
								scene3.confirmEmail = true;
								textRegisterConfirmEmail.setSelected(true);
							}
							// password
							if (x >= 15 && x <= 224 && y >= 198 && y <= 248)
							{
								scene3.password = true;
								textRegisterPassword.setSelected(true);
							}
							// confirm password
							if (x >= 15 && x <= 329 && y >= 280 && y <= 330)
							{
								scene3.confirmPassword = true;
								textRegisterConfirmPassword.setSelected(true);
							}
							//submit
							if (x >= 143 && x <= 457 && y >= 405 && y <= 480)
							{
								std::string e, ce, p, cp;
								e = textRegisterEmail.getText();
								ce = textRegisterConfirmEmail.getText();
								p = textRegisterPassword.getText();
								cp = textRegisterConfirmPassword.getText();
								if (e.empty() == false && ce.empty() == false && p.empty() == false && cp.empty() == false)
								{
									// send register to server

									if (e == ce && p == cp)
									{
										/*fout << e << "\n";
										fout << p << "\n";*/

										// Send data to the server
										std::string inter = "REGISTER";
										std::string message = inter + "||" + e + "||" + p;
										send(clientSocket, message.c_str(), message.size(), 0);

										// Receive data from the server
										char buffer[1024];
										int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
										if (bytesRead > 0) {
											buffer[bytesRead] = '\0';
											std::cout << "Received from server: " << buffer << std::endl;
											std::string messFromServer = std::string(buffer, bytesRead);
											struct demo messInfo = tokenize(messFromServer, "||");
											if (messInfo.arr[0] == "+OK") {
												hasLogin = true;
												nameLogin = e;
												scene = 4;
											}
											else if (messInfo.arr[0] == "-NO") {
												scene = 5;
											}
										}
									}

									
								}
							}
						}
						if (scene == 4) {
							// Kiểm tra nút Luyện tập
							updateListRoom = false;
							if (buttonTrain.getGlobalBounds().contains(sf::Vector2f(e.mouseButton.x, e.mouseButton.y)))
							{
								
								// Xử lý sự kiện cho nút Luyện tập ở đây
								std::cout << "Luyen tap";
								window.clear();
								window.setVisible(false);

								std::srand(std::time(0));
								auto tetris = std::make_shared<Tetris>(clientSocket, nameLogin);
								bool sendData1 = false;
								std::stringstream stream;
								std::string str_temp;
								// Đăng ký callback khi game over
								tetris->setGameOverCallback([&](int score) {
									// Xử lý khi game over, ví dụ: gửi tín hiệu về server
									if (!sendData1)
									{
										sendData1 = true;
										std::cout << "Game over! Sending signal to server...\n";
										// Send data to the server
										std::string inter = "TRAIN";
										stream << score;
										stream >> str_temp;
										std::string message = inter + "||" + str_temp + "||" +nameLogin;
										send(clientSocket, message.c_str(), message.size(), 0);
										window.setVisible(sendData1);
									}
									
								});


								tetris->run();
								

							}
							if (buttonFight.getGlobalBounds().contains(sf::Vector2f(e.mouseButton.x, e.mouseButton.y)))
							{

								// Xử lý sự kiện cho nút Solo ở đây
								std::cout << "Solo";
								scene = 6;
								

							}
							if (smallButton1.getGlobalBounds().contains(sf::Vector2f(e.mouseButton.x, e.mouseButton.y)))
							{

								// Xử lý sự kiện cho nút Rankking ở đây
								// Send data to the server
								std::string inter = "RANK";
								std::string message = inter + "||-||-" ;
								send(clientSocket, message.c_str(), message.size(), 0);

								// Receive data from the server
								std::vector<Player> leaderboard = receiveRank(clientSocket);
								// Display leaderboard
								window.clear();
								for (std::size_t i = 0; i < std::min(leaderboard.size(), static_cast<std::size_t>(10)); ++i) {

									textRank.emplace_back(std::to_string(i + 1) + ". " + leaderboard[i].name + ": " + (leaderboard[i].score), arial, 20, sf::Vector2f(100.0f, 150.0f + i * (60.0f)));;
									
								}
								scene = 15;

							}

							if (logOutButton.getGlobalBounds().contains(sf::Vector2f(e.mouseButton.x, e.mouseButton.y)))
							{
								if (hasLogin) {
									std::string inter = "LOGOUT";
									std::string message = inter + "||" + nameLogin + "|| -";
									send(clientSocket, message.c_str(), message.size(), 0);
									hasLogin = !hasLogin;
									scene = 1;
								}
							}
							
							if (searchButton.getGlobalBounds().contains(sf::Vector2f(e.mouseButton.x, e.mouseButton.y)))
							{
								sf::RenderWindow search(sf::VideoMode(600, 200), "SFML Search Bar");

								SearchBar searchBar(search);

								while (search.isOpen()) {
									sf::Event event;
									while (search.pollEvent(event)) {
										if (event.type == sf::Event::Closed) {
											search.close();
										}

										searchBar.handleEvents(event);
									}

									search.clear();
									searchBar.draw();
									search.display();

									/*if (searchBar.isTextInputActive()) {
										std::cout << "Search Text: " << searchBar.getSearchText() << std::endl;
									}*/

									if (searchBar.isFindButtonPressed()) {
										
										searchBar.setFindButtonPressed();

										std::cout << "Find button pressed!" << std::endl;
									}
								}
							}
					
						}
						if (scene == 6 ) {
							
							if (backButton.getGlobalBounds().contains(sf::Vector2f(e.mouseButton.x, e.mouseButton.y)))
							{
								scene = 4;
							}
							if (addRoom.isMouseOver(window)) {
								
								sf::RenderWindow Form2(sf::VideoMode(400, 200), "SFML Form");

								Form form1(Form2);
								
								while (Form2.isOpen()) {
									form1.handleInput();
									

									Form2.clear(sf::Color::White);

									form1.draw();
									
									Form2.display();

									if (form1.isSendButtonPressed()) {
										// Xử lý dữ liệu khi button "Gửi" được nhấp
										// Ví dụ: lấy dữ liệu từ form
										std::string name = form1.getName();
										std::string password = form1.getPassword();
										std::cout << name << "-" << password << "\n";

										std::string inter = "ADD_ROOM";
										
										std::string message = inter + "||" + name + "||" + password;
										send(clientSocket, message.c_str(), message.size(), 0);
										// Receive data from the server
										char buffer[1024];
										int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
										if (bytesRead > 0) {
											buffer[bytesRead] = '\0';
											std::cout << "Received from server: " << buffer << std::endl;
											std::string messFromServer = std::string(buffer, bytesRead);
											struct demo messInfo = tokenize(messFromServer, "||");
											if (messInfo.arr[0] == "+OK") {
												myRoom.name = name;
												scene = 7;
											}
											else if (messInfo.arr[0] == "-NO") {
												scene = 6;
												std::cout << "Error add room\n";
											}
										}

										// Reset trạng thái của button để tránh xử lý lặp lại
										form1.resetButtonState();
										Form2.close();
									}
								}
								Form2.close();
							
							}
							// Kiểm tra xem click vào button nào
							for (const auto& button : buttonList) {
								
								if (button.isMouseOver(window)) {
									std::string text_temp = button.text.getString();
									sf::RenderWindow Form2(sf::VideoMode(400, 200), "SFML Form");

									Form form1(Form2, text_temp);

									while (Form2.isOpen()) {
										form1.handleInput();


										Form2.clear(sf::Color::White);

										form1.draw();

										Form2.display();

										if (form1.isSendButtonPressed()) {
											// Xử lý dữ liệu khi button "Gửi" được nhấp
											// Ví dụ: lấy dữ liệu từ form
											std::string name = form1.getName();
											std::string password = form1.getPassword();
											std::cout << name << "-" << password << "\n";

											std::string inter = "JOIN_ROOM";

											std::string message = inter + "||" + name + "||" + password;
											send(clientSocket, message.c_str(), message.size(), 0);
											// Receive data from the server
											char buffer[1024];
											int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
											if (bytesRead > 0) {
												buffer[bytesRead] = '\0';
												std::cout << "Received from server: " << buffer << std::endl;
												std::string messFromServer = std::string(buffer, bytesRead);
												struct demo messInfo = tokenize(messFromServer, "||");
												if (messInfo.arr[0] == "+OK") {
													myRoom.name = name;
													scene = stoi(messInfo.arr[1]);
												}
												else if (messInfo.arr[0] == "-NO") {
													scene = 6;
													std::cout << "Error join room\n";
												}
											}

											// Reset trạng thái của button để tránh xử lý lặp lại
											form1.resetButtonState();
											Form2.close();
										}
									}
									Form2.close();
									std::cout << "Join room:" << text_temp << "\n";
								}
							}
						}
						if (scene == 7) {

							if (backButton.getGlobalBounds().contains(sf::Vector2f(e.mouseButton.x, e.mouseButton.y)))
							{
								scene = 6;
							}
							if (playButton.getGlobalBounds().contains(sf::Vector2f(e.mouseButton.x, e.mouseButton.y)))
							{
								std::string inter1 = "TETRIS_TEST";
								std::string str_temp;
								std::stringstream stream;

								stream << clientSocket;
								stream >> str_temp;

								std::string message = inter1 + "||" + myRoom.name + "||" + str_temp;
								send(clientSocket, message.c_str(), message.size(), 0);
								//char buffer[1024];
								//recv(clientSocket, buffer, sizeof(buffer), 0);

								window.clear();
								

								std::srand(std::time(0));
								auto tetris = std::make_shared<Tetris2>(clientSocket, myRoom.name, nameLogin);
								bool sendData2 = false;
								tetris->setGameOverCallback([&](int score) {
									// Xử lý khi game over, ví dụ: gửi tín hiệu về server
									if (!sendData2)
									{
										sendData2 = true;
										if (score == -10) {
											std::string inter12 = "OUTGAME";
											std::string message = inter12 + "||" + myRoom.name + "||" + std::to_string(clientSocket);
											send(clientSocket, message.c_str(), message.size(), 0);


										}
										std::cout << "Game over! Sending signal to server...\n";

									}

									});
								tetris->run();
								scene = 4;
								std::cout << "Play game";
							}
						}
						if (scene == 8) {

							if (backButton.getGlobalBounds().contains(sf::Vector2f(e.mouseButton.x, e.mouseButton.y)))
							{
								scene = 6;
							}
							if (readyButton.getGlobalBounds().contains(sf::Vector2f(e.mouseButton.x, e.mouseButton.y)))
							{
								// Receive data from the server
								char buffer[1024];
								int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
								if (bytesRead > 0) {
									buffer[bytesRead] = '\0';
									std::cout << "Received from server: " << buffer << std::endl;
									std::string messFromServer = std::string(buffer, bytesRead);
									struct demo messInfo = tokenize(messFromServer, "||");
									if (messInfo.arr[0] == "+OK") {
										
										
										bool sendData3 = false;
										std::srand(std::time(0));
										auto tetris = std::make_shared<Tetris2>(clientSocket, myRoom.name, nameLogin);
										
										tetris->setGameOverCallback([&](int score) {
											// Xử lý khi game over, ví dụ: gửi tín hiệu về server
											if (!sendData3)
											{
												sendData3 = true;
												if (score == -10) {

													std::string inter1 = "OUTGAME";
													std::string message = inter1 + "||" + myRoom.name + "||" + std::to_string(clientSocket);
													send(clientSocket, message.c_str(), message.size(), 0);


												}
												std::cout << "Game over! Sending signal to server...\n";
												
											}

										});
										tetris->run();
									}
								}
								
								std::cout << "Ready";
								scene = 4;
							}
						}
						if (scene == 15) {

							if (backButton.getGlobalBounds().contains(sf::Vector2f(e.mouseButton.x, e.mouseButton.y)))
							{
								scene = 4;
							}
							
						}
						if (scene == 5) {

							if (backButton.getGlobalBounds().contains(sf::Vector2f(e.mouseButton.x, e.mouseButton.y)))
							{
								scene = 1;
							}

						}
					}
				}
				if (e.type == Event::TextEntered)
				{
					if (scene == 2)
					{
						if (scene2.email == true)
						{
							if (Keyboard::isKeyPressed(Keyboard::Return))
							{
								textLoginEmail.setSelected(false);
								scene2.email = false;
							}
							else
							{
								textLoginEmail.typedOn(e);
							}
						}
						else if (scene2.password == true)
						{
							if (Keyboard::isKeyPressed(Keyboard::Return))
							{
								textLoginPassword.setSelected(false);
								scene2.password = false;
							}
							else
							{
								textLoginPassword.typedOn(e);
							}
						}
					}
					if (scene == 3)
					{
						if (scene3.email == true)
						{
							if (Keyboard::isKeyPressed(Keyboard::Return))
							{
								textRegisterEmail.setSelected(false);
								scene3.email = false;
							}
							else
							{
								textRegisterEmail.typedOn(e);
							}
						}
						else if (scene3.confirmEmail == true)
						{
							if (Keyboard::isKeyPressed(Keyboard::Return))
							{
								textRegisterConfirmEmail.setSelected(false);
								scene3.confirmEmail = false;
							}
							else
							{
								textRegisterConfirmEmail.typedOn(e);
							}
						}
						else if (scene3.password == true)
						{
							if (Keyboard::isKeyPressed(Keyboard::Return))
							{
								textRegisterPassword.setSelected(false);
								scene3.password = false;
							}
							else
							{
								textRegisterPassword.typedOn(e);
							}
						}
						else if (scene3.confirmPassword == true)
						{
							if (Keyboard::isKeyPressed(Keyboard::Return))
							{
								textRegisterConfirmPassword.setSelected(false);
								scene3.confirmPassword = false;
							}
							else
							{
								textRegisterConfirmPassword.typedOn(e);
							}
						}
					}
				}
			}
			window.clear(Color(104, 167, 92));
			if (scene == 1)
			{
				typing = true;
				login.setPosition(75, 87);
				window.draw(login);
				registerr.setPosition(75, 343);
				window.draw(registerr);
			}
			if (scene == 2)
			{
				
				Submit.setPosition(143, 405);
				window.draw(Submit);
				Password.setPosition(50, 235);
				window.draw(Password);
				Email.setPosition(50, 75);
				window.draw(Email);
				Back.setPosition(233, 530);
				window.draw(Back);


				textLoginEmail.setPosition({ 210,76 });
				textLoginEmail.drawTo(window);


				textLoginPassword.setPosition({ 269,237 });
				textLoginPassword.drawTo(window);

				if (!hasLogin) {
					if (typing) {
						textLoginEmail.resetText();
						textLoginEmail.setString("");
						textLoginPassword.resetText();
						textLoginPassword.setString("");
						typing = false;
					}
				}
			}
			if (scene == 3)
			{
				
				Submit.setPosition(143, 405);
				window.draw(Submit);
				Back.setPosition(233, 530);
				window.draw(Back);
				Email.setPosition(15, 34);
				window.draw(Email);
				ConfirmEmail.setPosition(15, 116);
				window.draw(ConfirmEmail);
				Password.setPosition(15, 198);
				window.draw(Password);
				ConfirmPassword.setPosition(15, 280);
				window.draw(ConfirmPassword);

				textRegisterEmail.setPosition({ 175, 35 });
				textRegisterEmail.drawTo(window);

				textRegisterConfirmEmail.setPosition({ 303,117 });
				textRegisterConfirmEmail.drawTo(window);

				textRegisterPassword.setPosition({ 234,199 });
				textRegisterPassword.drawTo(window);

				textRegisterConfirmPassword.setPosition({ 339,281 });
				textRegisterConfirmPassword.drawTo(window);
				if (!hasLogin) {
					if (typing) {
						textRegisterEmail.resetText();
						textRegisterEmail.setString("");
						textRegisterConfirmEmail.resetText();
						textRegisterConfirmEmail.setString("");
						textRegisterPassword.resetText();
						textRegisterPassword.setString("");
						textRegisterConfirmPassword.resetText();
						textRegisterConfirmPassword.setString("");
						typing = false;
					}
				}
			}
			if (scene == 4)
			{
				// Thiết lập các kích thước phần trên, giữa và dưới
				sf::FloatRect topRect(0.f, 0.f, 800.f, 80.f);
				sf::FloatRect middleRect(0.f, 80.f, 800.f, 420.f);
				sf::FloatRect bottomRect(0.f, 500.f, 800.f, 100.f);

				sf::RectangleShape topFrame(sf::Vector2f(topRect.width, topRect.height));
				sf::RectangleShape middleFrame(sf::Vector2f(middleRect.width, middleRect.height));
				sf::RectangleShape bottomFrame(sf::Vector2f(bottomRect.width, bottomRect.height));

				// Đặt màu cho frame
				topFrame.setFillColor(sf::Color::Blue);
				middleFrame.setFillColor(sf::Color::Green);
				bottomFrame.setFillColor(sf::Color::Red);

				// Đặt vị trí cho từng frame
				topFrame.setPosition(topRect.left, topRect.top);
				middleFrame.setPosition(middleRect.left, middleRect.top);
				bottomFrame.setPosition(bottomRect.left, bottomRect.top);


				// top frame
				logoSprite.setPosition(topRect.left+50, topRect.top + 10);
				logoSprite.setScale(0.3f, 0.3f);


				// middle frame
				searchButton.setPosition(middleRect.left + 450 , middleRect.top + 20);
				searchText.setString("Friend");
				searchText.setPosition(middleRect.left + 465, middleRect.top + 25);
				
				buttonTrain.setPosition(middleRect.left + 80, middleRect.top + 40);
				textButtonTrain.setCharacterSize(22);
				textButtonTrain.setFillColor(sf::Color::Black);
				textButtonTrain.setFont(arial);
				textButtonTrain.setString("Training");
				textButtonTrain.setPosition(middleRect.left + 115, middleRect.top + 65);

				buttonFight.setPosition(middleRect.left + 80, middleRect.top + 140);
				textButtonFight.setCharacterSize(22);
				textButtonFight.setFillColor(sf::Color::Black);
				textButtonFight.setFont(arial);
				textButtonFight.setString("Solo");
				textButtonFight.setPosition(middleRect.left + 135, middleRect.top + 165);

				
					
				logOutButton.setPosition(middleRect.left + 80, middleRect.top + 240);
				logOutButton.setFillColor(sf::Color::Yellow);
				logOutText.setPosition(middleRect.left + 125, middleRect.top + 265);


				// bottom frame 
				
				smallButton1.setPosition(bottomRect.left + 50.f, bottomRect.top + 50.f);
				smallButton2.setPosition(bottomRect.left + 280.f, bottomRect.top + 50.f);

				// Xóa cửa sổ để vẽ lại
				window.clear();

				// Vẽ frame
				window.draw(topFrame);
				window.draw(middleFrame);
				window.draw(bottomFrame);

				window.draw(logoSprite);
				window.draw(searchButton);
				window.draw(searchText);

				window.draw(buttonTrain);
				window.draw(buttonFight);
				window.draw(textButtonFight);
				window.draw(textButtonTrain);
				window.draw(logOutButton);
				window.draw(logOutText);


				window.draw(smallButton1);
				window.draw(smallButton2);
			}
			if (scene == 5)
			{
				loginFail.setPosition(193, 219);
				window.draw(loginFail);

				backButton.setPosition(233, 530);

				backText.setPosition(245, 540);
				window.draw(backButton);
				window.draw(backText);
			}
			if (scene == 6)
			{
				// Phần trên cùng (25%): Ảnh logo
				sf::Texture logoTexture;
				logoTexture.loadFromFile("images/logo.jpg");  // Đường dẫn đến ảnh logo
				sf::Sprite logoSprite(logoTexture);
				logoSprite.setScale(0.25f, 0.25f);
				
				
				addRoom.draw(window);
				

				if (!updateListRoom) {

					// send to server
					// Send data to the server
					updateListRoom = true;
					std::string inter = "LIST";
					std::string message = inter + "||-||-";
					send(clientSocket, message.c_str(), message.size(), 0);
					// Receive data from the server
					char buffer[1024];
					int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
					if (bytesRead > 0) {
						buffer[bytesRead] = '\0';
						std::cout << "Received from server: " << buffer << std::endl;
						std::string messFromServer = std::string(buffer, bytesRead);
						struct demo messInfo = tokenize(messFromServer, "||");
						if (messInfo.arr[0] == "+OK") {
							
							// Nhận vector từ server
							std::vector<GameRoom> receivedGameRooms = receiveGameRooms(clientSocket);
							// Phần 2: Hiển thị danh sách hoặc form tạo phòng

							
							// Hiển thị thông tin các phòng
							buttonList.clear();
							for (int i = 0; i < receivedGameRooms.size(); ++i) {
								// Add buttons to the list
								buttonList.emplace_back(receivedGameRooms[i].name, arial, 20, sf::Vector2f(100.0f, 150.0f + i*(60.0f)));
								//std::cout << receivedGameRooms[i].name << " -- " << receivedGameRooms[i].number << "\n";
								
								for (const auto& k : receivedGameRooms[i].sockets) {
									if (myRoom.name == receivedGameRooms[i].name) {
										myRoom = receivedGameRooms[i];
										std::cout << myRoom.name << myRoom.status << "\n";
									}
									std::cout << k << "--";
								}
								std::cout << "\n";
								
							}


							// Draw all buttons in the list
							
							for (const auto& button : buttonList) {
								button.draw(window);
							}
							

						}
						else if (messInfo.arr[0] == "-NO") {
							std::cout << "Khong co phong";
							MessageBox1 myMessageBox("No game rooms have been created yet ", arial, 50, sf::Vector2f(20, 500));
							myMessageBox.draw(window);
						}
					}
				}

				// Draw all buttons in the list
			
				for (const auto& button : buttonList) {
					button.draw(window);
				}
				// Phần 3:  Button Back
				
				backButton.setPosition(0, 550);
				backText.setPosition(10, 560);

				window.draw(logoSprite);

				window.draw(backButton);
				window.draw(backText);

				
			}
			if (scene == 7)
			{
				backText.setPosition(10, 560);
				window.draw(logoSprite);


				if (myRoom.status) {
					playButton.setPosition(50, 450);
					window.draw(playButton);
				}
				window.draw(backButton);
				window.draw(backText);
			}
			if (scene == 8)
			{
				backText.setPosition(10, 560);
				readyButton.setPosition(50, 450);

				window.draw(logoSprite);



				window.draw(readyButton);
				window.draw(backButton);
				window.draw(backText);
			}
			if (scene == 15)
			{
				backText.setPosition(10, 560);
				window.draw(backButton);
				window.draw(logoSprite);

				for (const auto& button : textRank) {
					button.draw(window);
				}

				
				window.draw(backText);
			}
        window.display();
       
    }

    // Close the client socket
    closesocket(clientSocket);

    // Cleanup Winsock
    WSACleanup();

    return 0;
}
