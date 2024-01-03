#include "formAddRoom.h"

Form::Form(sf::RenderWindow& window) : window(window), isButtonPressed(false) {
    
    font.loadFromFile("font/arial.ttf");

    nameLabel.setFont(font);
    nameLabel.setString("Name:");
    nameLabel.setCharacterSize(20);
    nameLabel.setPosition(50, 50);
    nameLabel.setFillColor(sf::Color::Black);

    passwordLabel.setFont(font);
    passwordLabel.setString("Password:");
    passwordLabel.setCharacterSize(20);
    passwordLabel.setPosition(50, 100);
    passwordLabel.setFillColor(sf::Color::Black);

    nameText.setFont(font);
    nameText.setCharacterSize(20);
    nameText.setPosition(150, 50);
    nameText.setFillColor(sf::Color::Black);

    passwordText.setFont(font);
    passwordText.setCharacterSize(20);
    passwordText.setPosition(150, 100);
    passwordLabel.setFillColor(sf::Color::Black);

    initButton();
}

void Form::initButton() {
    sendButton.setSize(sf::Vector2f(100, 50));
    sendButton.setPosition(250, 150);
    sendButton.setFillColor(sf::Color::Green);
}

void Form::handleInput() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        else if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));

                if (sendButton.getGlobalBounds().contains(mousePos)) {
                    // Xử lý sự kiện khi button "Gửi" được nhấp
                    isButtonPressed = true;
                }
            }
        }
        else if (event.type == sf::Event::TextEntered && window.hasFocus()) {
            if (event.text.unicode < 128) {
                if (nameText.getGlobalBounds().contains(window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)))) {
                    if (event.text.unicode == 8 && !name.empty()) {
                        // Xử lý khi người dùng nhấn phím Backspace
                        name.pop_back();
                    }
                    else if (event.text.unicode != 8) {
                        // Thêm ký tự vào trường "Name"
                        name += static_cast<char>(event.text.unicode);
                    }
                    nameText.setString(name);
                }
                else if (passwordText.getGlobalBounds().contains(window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)))) {
                    if (event.text.unicode == 8 && !password.empty()) {
                        // Xử lý khi người dùng nhấn phím Backspace
                        password.pop_back();
                    }
                    else if (event.text.unicode != 8) {
                        // Thêm ký tự vào trường "Password"
                        password += static_cast<char>(event.text.unicode);
                    }
                    passwordText.setString(password);
                }
            }
        }
    }
}
void Form::draw() {
    window.draw(nameLabel);
    window.draw(passwordLabel);
    window.draw(nameText);
    window.draw(passwordText);
    window.draw(sendButton);
}

std::string Form::getName() const {
    return name;
}

std::string Form::getPassword() const {
    return password;
}

bool Form::isSendButtonPressed() {
    return isButtonPressed;
}

void Form::resetButtonState() {
    isButtonPressed = false;
}