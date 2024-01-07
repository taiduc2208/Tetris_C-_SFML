#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

class Room {
public:
    sf::Text text;
    sf::RectangleShape border;
    sf::Vector2f positionT;


    Room(const std::string& buttonText, const sf::Font& font, unsigned int characterSize, const sf::Vector2f& position)
        : text(buttonText, font, characterSize) {
        
        text.setFillColor(sf::Color::Black);
        text.setPosition(position);

        positionT = position;

        // Set up border
        sf::FloatRect textBounds = text.getGlobalBounds();
        border.setSize(sf::Vector2f(textBounds.width + 20.0f, textBounds.height + 12.0f));
        border.setPosition(position.x - 10.0f, position.y - 5.0f);
        border.setOutlineThickness(2.0f);
        border.setOutlineColor(sf::Color::White);
    }

    sf::Vector2f getPosition() {
        return positionT;
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(border);
        window.draw(text);
    }
    void setColor(sf::Color& color)  {
        text.setFillColor(color);
    }
    void setBorderColor(sf::Color& color) {
        border.setFillColor(color);
    }
    void setPosition(sf::Vector2f& vector) {
        text.setPosition(vector);
    }
    void setChoosenColor() {
        border.setFillColor(sf::Color::Yellow);
        text.setFillColor(sf::Color(128, 0, 128));
    }
    
    std::string getString() {
        return text.getString();
    }
    bool isMouseOver(const sf::RenderWindow& window) const {
        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
        sf::Vector2f borderPosition = border.getPosition();
        sf::Vector2f rectSize = border.getSize();

        return (mousePosition.x >= borderPosition.x && mousePosition.x <= borderPosition.x + rectSize.x &&
            mousePosition.y >= borderPosition.y && mousePosition.y <= borderPosition.y + rectSize.y);
    }
};