#include "tetris.h"
#include <iostream>

Tetris::~Tetris() {
    // Hủy các tài nguyên
    // ...
}

Tetris::Tetris(SOCKET socket, std::string name, int levelTetris) {

    level = levelTetris;
    std::cout << "  level:\t" << level << "\n";
    gameSock = socket;
    nameLogin = name;
    area.resize(lines);
    for (std::size_t i{}; i < area.size(); ++i) {
        area[i].resize(cols);
    }

    forms = {
      {1,3,5,7}, // I
      {2,4,5,7}, // Z
      {3,5,4,6}, // S
      {3,5,4,7}, // T
      {2,3,5,7}, // L
      {3,5,7,6}, // J
      {2,3,4,5}, // O
    };

    window = std::make_shared<sf::RenderWindow>(
        sf::VideoMode(720, 720),
        "Tetris (remix)",
        sf::Style::Titlebar | sf::Style::Close
    );
    window->setPosition(sf::Vector2i(100, 100));

    tiles.loadFromFile("images/squares.png");
    sprite = std::make_shared<sf::Sprite>();
    sprite->setTexture(tiles);
    sprite->setTextureRect(sf::IntRect(0, 0, 36, 36));

    predictAbu = std::make_shared<sf::Sprite>();
    predictAbu->setTexture(tiles);
    predictAbu->setTextureRect(sf::IntRect(0, 0, 36, 36));

    bg.loadFromFile("images/background1.png");
    background = std::make_shared<sf::Sprite>();
    background->setTexture(bg);

    dirx = score = { 0 };
    rotate = gameover = { false };
    timercount = { 0.f };
    delay = { 0.3f };
    color = { 1 };

    std::uint32_t number = std::rand() % shapes;
    for (std::size_t i{}; i < squares; ++i) {
        z[i].x = forms[number][i] % 2;
        z[i].y = forms[number][i] / 2;
    }

    number = std::rand() % shapes;
    for (std::size_t i{}; i < squares; ++i) {
        predict[i].x = forms[number][i] % 2;
        predict[i].y = forms[number][i] / 2;
        std::cout << (predict[i].x + 12) * 36 << "--" << (predict[i].y + 12) * 36 << "\n";
    }


    font.loadFromFile("font/font.ttf");
    txtScore.setFont(font);
    txtScore.setPosition(100.f, 10.f);
    txtScore.setString("SCORE: " + std::to_string(score));
    txtScore.setCharacterSize(30);
    txtScore.setOutlineThickness(3);

    txtGameOver.setFont(font);
    txtGameOver.setPosition(30.f, 300.f);
    txtGameOver.setString("GAME OVER");
    txtGameOver.setCharacterSize(50);
    txtGameOver.setOutlineThickness(3);

    txtLevel.setFont(font);
    txtLevel.setPosition(460.f, 10.f);
    txtLevel.setString("LEVEL :" + std::to_string(level + 1));
    txtLevel.setCharacterSize(50);
    txtLevel.setOutlineThickness(3);

    textNext.setFont(font);
    textNext.setPosition(400.f, 360.f);
    textNext.setString("Next");
    textNext.setCharacterSize(50);
    textNext.setOutlineThickness(3);

}

void Tetris::events() {

    float time = clock.getElapsedTime().asSeconds();
    clock.restart();
    timercount += time;

    auto e = std::make_shared<sf::Event>();
    while (window->pollEvent(*e)) {
        if (e->type == sf::Event::Closed) {
            gameOverCallback(score);
            window->close();
        }

        if (e->type == sf::Event::KeyPressed) {
            if (e->key.code == sf::Keyboard::Up) {
                rotate = true;
            }
            else if (e->key.code == sf::Keyboard::Right) {
                ++dirx;
            }
            else if (e->key.code == sf::Keyboard::Left) {
                --dirx;
            }
            else if (gameover && sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
                sendData = false;
                reset();
                gameover = false;
            }
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        delay = 0.05f;
    }
}

void Tetris::draw() {
    window->clear(sf::Color::Black);
    window->draw(*background);
    sf::Color greyColor(128, 128, 128);
    sf::FloatRect topRect(360.f, 0.f, 360.f, 400.f);
    sf::FloatRect bottomRect(360.f, 600.f, 360.f, 120.f);
    sf::RectangleShape topFrame(sf::Vector2f(topRect.width, topRect.height));
    sf::RectangleShape bottomFrame(sf::Vector2f(bottomRect.width, bottomRect.height));

    topFrame.setFillColor(greyColor);
    bottomFrame.setFillColor(greyColor);

    // Đặt vị trí cho từng frame
    topFrame.setPosition(topRect.left, topRect.top);
    bottomFrame.setPosition(bottomRect.left, bottomRect.top);
    window->draw(topFrame);
    window->draw(bottomFrame);

    for (std::size_t i{}; i < lines; ++i) {
        for (std::size_t j{}; j < cols; ++j) {
            if (area[i][j] != 0) {
                sprite->setTextureRect(sf::IntRect(area[i][j] * 36, 0, 36, 36));
                sprite->setPosition(j * 36, i * 36);
                window->draw(*sprite);

            }
        }
    }

    for (std::size_t i{}; i < squares; ++i) {
        sprite->setTextureRect(sf::IntRect(color * 36, 0, 36, 36));
        sprite->setPosition(z[i].x * 36, z[i].y * 36);

        predictAbu->setTextureRect(sf::IntRect(color * 36, 0, 36, 36));
        predictAbu->setPosition((predict[i].x + 12) * 36, (predict[i].y + 12) * 36);

        

        window->draw(*predictAbu);
        window->draw(*sprite);
    }

    window->draw(txtScore);
    window->draw(txtLevel);
    window->draw(textNext);
    if (gameover) {
        window->draw(txtGameOver);
    }
    window->display();
}
void Tetris::setGameOverCallback(std::function<void(int)> callback) {
    gameOverCallback = callback;
}

void Tetris::reset() {
    // Reset relevant variables to their initial values
    dirx = score = { 0 };
    rotate = gameover = { false };
    timercount = { 0.f };
    delay = { 0.3f };
    color = { 1 };

    // Reset the game area
    for (std::size_t i = 0; i < lines; ++i) {
        for (std::size_t j = 0; j < cols; ++j) {
            area[i][j] = 0;
        }
    }
    setGameOverCallback([&](int score) {
        if (!sendData) {
            sendData = true;
            // Send data to the server
            std::string inter = "TRAIN";
            std::string message = inter + "||" + std::to_string(score) + "||" + nameLogin;
            send(gameSock, message.c_str(), message.size(), 0);

        }
        });
    // Update the score display
    txtScore.setString("SCORE: " + std::to_string(score));
}


void Tetris::run() {
    while (window->isOpen()) {
        events();
        if (!gameover) {
            changePosition();
            setRotate();
            moveToDown();
            setScore();
            resetValues();
        }
        else {
            if (gameOverCallback) {
                gameOverCallback(score);
            }
        }
        draw();
    }
}


int Tetris::getScore() const {
    return score;
}

void Tetris::moveToDown() {
    if (timercount > delay) {
        for (std::size_t i{}; i < squares; ++i) {
            k[i] = z[i];
            ++z[i].y;
        }

        if (maxLimit()) {

            for (std::size_t i{}; i < squares; ++i) {
                area[k[i].y][k[i].x] = color;
            }


            for (std::size_t i{}; i < squares; ++i) {
                z[i].x = predict[i].x;
                z[i].y = predict[i].y;
            }
            color = std::rand() % shapes + 1;
            std::uint32_t number = std::rand() % shapes;
            for (std::size_t i{}; i < squares; ++i) {
                predict[i].x = forms[number][i] % 2;
                predict[i].y = forms[number][i] / 2;
                
            }
        }

        timercount = 0;
    }

}

void Tetris::setRotate() {
    if (rotate) {
        Coords coords = z[1];
        for (std::size_t i{}; i < squares; ++i) {
            int x = z[i].y - coords.y;
            int y = z[i].x - coords.x;

            z[i].x = coords.x - x;
            z[i].y = coords.y + y;
        }

        if (maxLimit()) {
            for (std::size_t i{}; i < squares; ++i) {
                z[i] = k[i];
            }
        }
    }
}

void Tetris::resetValues() {
    dirx = 0;
    rotate = false;
    
    
    delay = 0.3f - level* 0.01f;
    
    //std::cout << score << ":" << (score > 1) << delay << "\n";
}

void Tetris::changePosition() {
    for (std::size_t i{}; i < squares; ++i) {
        k[i] = z[i];
        z[i].x += dirx;
    }

    if (maxLimit()) {
        for (std::size_t i{}; i < squares; ++i) {
            z[i] = k[i];
        }
    }
}

bool Tetris::maxLimit() {
    for (std::size_t i{}; i < squares; ++i) {
        if (z[i].x < 0 ||
            z[i].x >= cols ||
            z[i].y >= lines ||
            area[z[i].y][z[i].x]) {
            return true;
        }
    }
    return false;
}

void Tetris::setScore() {
    std::uint32_t match = lines - 1;
    int countLines = 0;
    bool checkPrint = false;
    for (std::size_t i = match; i >= 1; --i) {
        std::uint32_t sum{};
        for (std::size_t j{}; j < cols; ++j) {
            if (area[i][j]) {
                if (i == 1) {
                    gameover = true;
                }
                ++sum;
            }
            area[match][j] = area[i][j];
        }
        if (sum < cols) {
            --match;
        }
        else {
            countLines++;
            checkPrint = true;
            if (countLines >= 2) {
                score += (countLines - 1);
            }
            txtScore.setString("SCORE: " + std::to_string(++score));
            txtLevel.setString("LEVEL: " + std::to_string(++level));
            std::cout << "score:\t" << score << "---speed:\t" << (30/delay) << "\n";
        }
    }
    
    if (checkPrint) {
        checkPrint = false;
        std::cout << "number of lines clear:\t" << countLines << "\n";
    }
}