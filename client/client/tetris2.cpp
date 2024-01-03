#include "tetris2.h"

Tetris2::~Tetris2() {
    // Hủy các tài nguyên
    // ...
}
sf::RectangleShape divider;
Tetris2::Tetris2() {

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
        sf::VideoMode(724, 720),
        "Tetris 2 player (remix)",
        sf::Style::Titlebar | sf::Style::Close
    );
    window->setPosition(sf::Vector2i(300, 100));

    tiles.loadFromFile("images/squares.png");
    sprite = std::make_shared<sf::Sprite>();
    sprite->setTexture(tiles);
    sprite->setTextureRect(sf::IntRect(0, 0, 36, 36));

    bg.loadFromFile("images/background1.png");
    background = std::make_shared<sf::Sprite>();
    background->setTexture(bg);
    
    bg1.loadFromFile("images/background1.png");
    background1 = std::make_shared<sf::Sprite>();
    background1->setTexture(bg1);

    background->setPosition(0, 0);
    background1->setPosition(360, 0);


    // Tạo divider
    
    divider.setSize(sf::Vector2f(4,720));
    divider.setFillColor(sf::Color::Blue);
    divider.setPosition(360, 0);

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
}

void Tetris2::events() {

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
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        delay = 0.05f;
    }
}

void Tetris2::draw() {
    window->clear(sf::Color::Black);
    window->draw(*background);
    window->draw(*background1);
    window->draw(divider);

    for (std::size_t i{}; i < lines; ++i) {
        for (std::size_t j{}; j < cols; ++j) {
            if (area[i][j] != 0) {
                sprite->setTextureRect(sf::IntRect(area[i][j] * 36, 0, 36, 36));
                sprite->setPosition(j * 36, i * 36);
                window->draw(*sprite);

            }
        }
    }
    for (std::size_t i{}; i < lines; ++i) {
        for (std::size_t j{}; j < cols; ++j) {
            if (area[i][j] != 0) {
                sprite->setTextureRect(sf::IntRect(area[i][j] * 36, 0, 36, 36));
                sprite->setPosition((j+cols) * 36, i * 36);
                window->draw(*sprite);

            }
        }
    }

    for (std::size_t i{}; i < squares; ++i) {
        sprite->setTextureRect(sf::IntRect(color * 36, 0, 36, 36));
        sprite->setPosition(z[i].x * 36, z[i].y * 36);
        window->draw(*sprite);
    }

    
    for (std::size_t i{}; i < squares; ++i) {
        sprite->setTextureRect(sf::IntRect(color * 36, 0, 36, 36));
        sprite->setPosition((z[i].x + cols) * 36, z[i].y * 36);
        window->draw(*sprite);
    }

   

    window->draw(txtScore);
    if (gameover) {
        window->draw(txtGameOver);
    }
    window->display();
}


void Tetris2::run() {
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

void Tetris2::setGameOverCallback(std::function<void(int)> callback) {
    gameOverCallback = callback;
}
int Tetris2::getScore() const {
    return score;
}

void Tetris2::moveToDown() {
    if (timercount > delay) {
        for (std::size_t i{}; i < squares; ++i) {
            k[i] = z[i];
            ++z[i].y;
        }

        if (maxLimit()) {

            for (std::size_t i{}; i < squares; ++i) {
                area[k[i].y][k[i].x] = color;
            }

            color = std::rand() % shapes + 1;
            std::uint32_t number = std::rand() % shapes;
            for (std::size_t i{}; i < squares; ++i) {
                z[i].x = forms[number][i] % 2;
                z[i].y = forms[number][i] / 2;
            }
        }

        timercount = 0;
    }

}

void Tetris2::setRotate() {
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

void Tetris2::resetValues() {
    dirx = 0;
    rotate = false;
    delay = 0.3f;
}

void Tetris2::changePosition() {
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

bool Tetris2::maxLimit() {
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

void Tetris2::setScore() {
    std::uint32_t match = lines - 1;
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
            txtScore.setString("SCORE: " + std::to_string(++score));
        }
    }
}