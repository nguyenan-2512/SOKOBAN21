#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

enum class MenuState {
    MENU,
    SELECTION_MAP,
    PLAYING,
    EXIT
};

class Menu {
private:
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    sf::Font font;
    std::vector<sf::Text> menuItems;
    sf::RectangleShape playButton;
    sf::RectangleShape exitButton;
    sf::Text playText;
    sf::Text exitText;
    int selectedItem;
    MenuState currentState;

    sf::Texture selectMapBackgroundTexture;
    sf::Sprite selectMapBackgroundSprite;
    std::vector<sf::RectangleShape> mapButtons;
    std::vector<sf::Text> mapItems;
    int selectedMap = -1;
    int highlightedMapIndex = 0;

public:
    Menu();
    ~Menu();
    bool loadResources();
    void handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void update();
    void draw(sf::RenderWindow& window);
    MenuState getState() const;
    void setState(MenuState state);
    void reset();
    int getSelectedMap() const;  // Thêm hàm này để lấy map được chọn

private:
    void setupButtons();
    void setupTexts();
    bool isMouseOverButton(const sf::RectangleShape& button, sf::Vector2i mousePos);
    void setupMapItems(sf::RenderWindow& window);
    void drawSelectionMap(sf::RenderWindow& window);
};