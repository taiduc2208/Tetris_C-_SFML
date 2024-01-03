#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class Form {
public:
    Form(sf::RenderWindow& window);

    void handleInput();
    void draw();
    bool isSendButtonPressed();
    void resetButtonState();

    std::string getName() const;
    std::string getPassword() const;

private:
    sf::RenderWindow& window;
    sf::Font font;
    sf::Text nameLabel;
    sf::Text passwordLabel;
    sf::Text nameText;
    sf::Text passwordText;
    sf::RectangleShape sendButton;

    std::string name;
    std::string password;
    bool isButtonPressed;

    void initButton();
    
};