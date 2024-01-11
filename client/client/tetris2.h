#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <functional>
#include <winsock2.h>
#include <ws2tcpip.h>


class Tetris2 {

    
    static const std::uint32_t lines{ 20 };
    static const std::uint32_t cols{ 10 };
    static const std::uint32_t squares{ 4 };
    static const std::uint32_t shapes{ 7 };

    std::vector<std::vector<std::uint32_t>> area;
    std::vector<std::vector<std::uint32_t>> forms;

    struct Coords {
        std::uint32_t x, y;
    } z[squares], k[squares], predict[squares];

    std::shared_ptr<sf::RenderWindow> window;
    sf::Texture tiles, bg, bg1;
    std::shared_ptr<sf::Sprite> sprite, background, background1, predictAbu;
    sf::Clock clock;
    sf::Font  font;
    sf::Text txtScore, txtGameOver, textResult;

    int dirx, color, score, level;
    bool rotate, gameover, sendResult;
    float timercount, delay;
    std::string nameLog, nameEnermy;
    

protected:
    void events();
    void draw();
    void moveToDown();
    void setRotate();
    void resetValues();
    void changePosition();
    bool maxLimit();
    void setScore();
    std::function<void(int)> gameOverCallback;

public:
    std::vector<std::vector<std::uint32_t>> areaEnermy;
    Tetris2(SOCKET socket, std::string name, std::string nameLog, std::string nameEnermy);
    ~Tetris2();
    void run();
    void setGameOverCallback(std::function<void(int)> callback);
    int getScore() const;
    void receiveData();
    bool sendVector2D(SOCKET socket, const std::vector<std::vector<std::uint32_t>>& data);
    SOCKET gameSocket;
    std::string roomName;
};