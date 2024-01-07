
#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include "textbox.h"

class SearchBar {
public:
    SearchBar(sf::RenderWindow& window): window(window)
    {
        if (!font.loadFromFile("font/arial.ttf")) {
            // Xử lý lỗi nếu không thể tải font
            std::cerr << "Error loading font\n";
        }

        label.setFont(font);
        label.setString("Search: ");
        label.setCharacterSize(20);
        label.setPosition(10, 50);

        textInput.setFont(font);
        textInput.setCharacterSize(20);
        textInput.setPosition(110, 50);


        findButton.setSize(sf::Vector2f(100, 50));
        findButton.setPosition(350, 40);
        findButton.setFillColor(sf::Color::Green);

        findText.setFont(font);
        findText.setString("Add");
        findText.setCharacterSize(20);
        findText.setPosition(365, 55);

        textResult.setFont(font);
        textResult.setCharacterSize(20);
        textResult.setPosition(20, 75);
        
        
    }

    void draw() {
        window.draw(label);
        window.draw(textInput);
        window.draw(findButton);
        window.draw(findText);
        
    }
    void drawResult(int type) {
        textResult.setString("");
        if (type == 1) {
            textResult.setString("Da gui loi moi ket ban");
            textResult.setFillColor(sf::Color::Green);
        }
        else if(type == 2) {
            textResult.setString("Ten user khong ton tai");
            textResult.setFillColor(sf::Color::Red);
        }
        window.draw(textResult);
    }
    bool isTextInputActive() const {
        return textInputActive;
    }

    bool isFindButtonPressed() const {
        return findButtonPressed;
    }

    void setFindButtonPressed() {
        findButtonPressed = !findButtonPressed;
    }

    std::string getSearchText() const {
        return searchText;
    }

    void handleEvents(sf::Event event) {
        if (event.type == sf::Event::TextEntered) {
            if (textInputActive) {
                if (event.text.unicode == 13) { // Enter key
                    textInputActive = false;
                }
                else if (event.text.unicode == 8 && searchText.length() > 0) { // Backspace key
                    searchText.pop_back();
                    textInput.setString(searchText);
                }
                else {
                    searchText += static_cast<char>(event.text.unicode);
                    textInput.setString(searchText);
                }
            }
        }
        else if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                if (findButton.getGlobalBounds().contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y))) {
                    findButtonPressed = true;
                }
                else {
                    textInputActive = true;
                }
            }
        }
    }


private:
    sf::RenderWindow& window;
    sf::Font font;
    sf::Text label;
    sf::Text textInput;
    sf::RectangleShape findButton;
    sf::Text findText;
    sf::Text textResult;

    bool textInputActive = false;
    bool findButtonPressed = false;
    std::string searchText;
};
