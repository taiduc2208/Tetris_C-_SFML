﻿#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <functional>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string.h>


class Tetris {

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
    sf::Texture tiles, bg;
    std::shared_ptr<sf::Sprite> sprite, background, predictAbu;
    sf::Clock clock;
    sf::Font  font;
    sf::Text txtScore, txtGameOver, txtLevel, textNext;

    int dirx, color, score, level;
    bool rotate, gameover, sendData;
    float timercount, delay;
    SOCKET gameSock;
    std::string nameLogin;

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
    Tetris(SOCKET socket, std::string name, int levelTetris);
    ~Tetris();
    void run();
    void setGameOverCallback(std::function<void(int)> callback);
    int getScore() const;
    void reset();

};