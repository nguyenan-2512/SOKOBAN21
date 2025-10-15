#include "Menu.h"
#include <iostream>
using namespace std;

Menu::Menu() : selectedItem(0), currentState(MenuState::MENU) {
}

Menu::~Menu() {
}

bool Menu::loadResources() {
    // Load background texture
    if (!backgroundTexture.loadFromFile("D:\\PBL2\\SOKOBAN2\\SOKOBAN1\\SOKOBAN\\SOKOBAN\\images\\Menu (2).jpg")) {
        std::cerr << "Failed to load menu background texture!" << std::endl;
        backgroundTexture.create(800, 600);
        sf::Uint8* pixels = new sf::Uint8[800 * 600 * 4];
        for (int i = 0; i < 800 * 600 * 4; i += 4) {
            pixels[i] = 50;
            pixels[i + 1] = 80;
            pixels[i + 2] = 120;
            pixels[i + 3] = 255;
        }
        backgroundTexture.update(pixels);
        delete[] pixels;
    }
    backgroundSprite.setTexture(backgroundTexture);

    // Load select map background
    if (!selectMapBackgroundTexture.loadFromFile("D:\\PBL2\\SOKOBAN2\\SOKOBAN1\\SOKOBAN\\SOKOBAN\\images\\Selectmap.png")) {
        std::cerr << "Failed to load select map background!" << std::endl;
    }
    selectMapBackgroundSprite.setTexture(selectMapBackgroundTexture);

    // Scale background menu to fit window (800x600)
    sf::Vector2u textureSize = backgroundTexture.getSize();
    if (textureSize.x > 0 && textureSize.y > 0) {
        float scaleX = 800.0f / textureSize.x;
        float scaleY = 600.0f / textureSize.y;
        backgroundSprite.setScale(scaleX, scaleY);
    }

    // Scale select map background to fit window
    sf::Vector2u selectMapTextureSize = selectMapBackgroundTexture.getSize();
    if (selectMapTextureSize.x > 0 && selectMapTextureSize.y > 0) {
        float scaleX = 800.0f / selectMapTextureSize.x;
        float scaleY = 600.0f / selectMapTextureSize.y;
        selectMapBackgroundSprite.setScale(scaleX, scaleY);
    }

    // Load font
    if (!font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
        std::cerr << "Failed to load font, using default font" << std::endl;
    }

    setupButtons();
    setupTexts();

    return true;
}

void Menu::setupButtons() {
    // Setup Play button
    playButton.setSize(sf::Vector2f(200, 60));
    playButton.setPosition(300, 200);
    playButton.setFillColor(sf::Color(70, 130, 180, 200));
    playButton.setOutlineThickness(3);
    playButton.setOutlineColor(sf::Color::White);

    // Setup Exit button
    exitButton.setSize(sf::Vector2f(200, 60));
    exitButton.setPosition(300, 300);
    exitButton.setFillColor(sf::Color(180, 70, 70, 200));
    exitButton.setOutlineThickness(3);
    exitButton.setOutlineColor(sf::Color::White);
}

void Menu::setupTexts() {
    // Play button text
    playText.setFont(font);
    playText.setString("PLAY");
    playText.setCharacterSize(24);
    playText.setFillColor(sf::Color::White);
    playText.setStyle(sf::Text::Bold);

    sf::FloatRect playBounds = playText.getLocalBounds();
    playText.setPosition(400 - playBounds.width / 2, 215);

    // Exit button text
    exitText.setFont(font);
    exitText.setString("EXIT");
    exitText.setCharacterSize(24);
    exitText.setFillColor(sf::Color::White);
    exitText.setStyle(sf::Text::Bold);

    sf::FloatRect exitBounds = exitText.getLocalBounds();
    exitText.setPosition(400 - exitBounds.width / 2, 315);
}

void Menu::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    if (currentState == MenuState::MENU) {
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);

            if (isMouseOverButton(playButton, mousePos)) {
                currentState = MenuState::SELECTION_MAP;
                setupMapItems(window);
            }
            else if (isMouseOverButton(exitButton, mousePos)) {
                currentState = MenuState::EXIT;
            }
        }

        // Handle keyboard input
        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
            case sf::Keyboard::Up:
                selectedItem = (selectedItem - 1 + 2) % 2;
                break;
            case sf::Keyboard::Down:
                selectedItem = (selectedItem + 1) % 2;
                break;
            case sf::Keyboard::Enter:
                if (selectedItem == 0) {
                    currentState = MenuState::SELECTION_MAP;
                    setupMapItems(window);
                }
                else {
                    currentState = MenuState::EXIT;
                }
                break;
            case sf::Keyboard::Escape:
                currentState = MenuState::EXIT;
                break;
            }
        }

        // Handle mouse hover effects
        if (event.type == sf::Event::MouseMoved) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);

            if (isMouseOverButton(playButton, mousePos)) {
                selectedItem = 0;
                playButton.setFillColor(sf::Color(100, 160, 210, 200));
            }
            else {
                playButton.setFillColor(sf::Color(70, 130, 180, 200));
            }

            if (isMouseOverButton(exitButton, mousePos)) {
                selectedItem = 1;
                exitButton.setFillColor(sf::Color(210, 100, 100, 200));
            }
            else {
                exitButton.setFillColor(sf::Color(180, 70, 70, 200));
            }
        }
    }
    // ============ BẮT ĐẦU PHẦN SỬA ĐỔI ============
    else if (currentState == MenuState::SELECTION_MAP) {
        // Xử lý click chuột
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);

            for (size_t i = 0; i < mapButtons.size(); ++i) {
                if (isMouseOverButton(mapButtons[i], mousePos)) {
                    selectedMap = static_cast<int>(i);
                    std::cout << "Selected Map: " << (i + 1) << std::endl;
                    currentState = MenuState::PLAYING;
                    break;
                }
            }
        }

        // Xử lý phím điều hướng - PHẦN MỚI THÊM
        if (event.type == sf::Event::KeyPressed) {
            const int numMaps = 10;

            switch (event.key.code) {
            case sf::Keyboard::Left:
            case sf::Keyboard::A:
                // Di chuyển sang trái nếu không ở cột đầu tiên
                if (highlightedMapIndex % 5 > 0) {
                    highlightedMapIndex--;
                }
                break;

            case sf::Keyboard::Right:
            case sf::Keyboard::D:
                // Di chuyển sang phải nếu không ở cột cuối và chưa vượt quá số map
                if (highlightedMapIndex % 5 < 4 && highlightedMapIndex < numMaps - 1) {
                    highlightedMapIndex++;
                }
                break;

            case sf::Keyboard::Up:
            case sf::Keyboard::W:
                // Di chuyển lên hàng trên nếu đang ở hàng dưới
                if (highlightedMapIndex >= 5) {
                    highlightedMapIndex -= 5;
                }
                break;

            case sf::Keyboard::Down:
            case sf::Keyboard::S:
                // Di chuyển xuống hàng dưới nếu đang ở hàng trên
                if (highlightedMapIndex < 5) {
                    highlightedMapIndex += 5;
                }
                break;

            case sf::Keyboard::Enter:
            case sf::Keyboard::Space:
                // Chọn map hiện tại
                selectedMap = highlightedMapIndex;
                std::cout << "Selected Map: " << (selectedMap + 1) << std::endl;
                currentState = MenuState::PLAYING;
                break;

            case sf::Keyboard::Escape:
                // Quay lại menu chính
                currentState = MenuState::MENU;
                break;
            }
        }

        // Xử lý hover chuột - CẬP NHẬT
        if (event.type == sf::Event::MouseMoved) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);

            for (size_t i = 0; i < mapButtons.size(); ++i) {
                if (isMouseOverButton(mapButtons[i], mousePos)) {
                    highlightedMapIndex = static_cast<int>(i);
                    break;
                }
            }
        }
    }
    // ============ KẾT THÚC PHẦN SỬA ĐỔI ============
}
void Menu::update() {
    if (currentState == MenuState::MENU) {
        // Update button colors based on selection
        if (selectedItem == 0) {
            playButton.setFillColor(sf::Color(100, 160, 210, 200));
            exitButton.setFillColor(sf::Color(180, 70, 70, 200));
        }
        else {
            playButton.setFillColor(sf::Color(70, 130, 180, 200));
            exitButton.setFillColor(sf::Color(210, 100, 100, 200));
        }
    }
}

void Menu::draw(sf::RenderWindow& window) {
    if (currentState == MenuState::MENU) {
        window.draw(backgroundSprite);
        window.draw(playButton);
        window.draw(exitButton);
        window.draw(playText);
        window.draw(exitText);
    }
    else if (currentState == MenuState::SELECTION_MAP) {
        drawSelectionMap(window);
    }
}

MenuState Menu::getState() const {
    return currentState;
}

void Menu::setState(MenuState state) {
    currentState = state;
}

void Menu::reset() {
    currentState = MenuState::MENU;
    selectedItem = 0;
}

int Menu::getSelectedMap() const {
    return selectedMap;
}

bool Menu::isMouseOverButton(const sf::RectangleShape& button, sf::Vector2i mousePos) {
    sf::FloatRect buttonBounds = button.getGlobalBounds();
    return buttonBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
}

void Menu::setupMapItems(sf::RenderWindow& window) {
    mapButtons.clear();
    mapItems.clear();
    highlightedMapIndex = 0;  // THÊM DÒNG NÀY - Reset về map đầu tiên

    const int numMaps = 10;

    // Điều chỉnh các thông số để khớp với ảnh background (1024x1024 scale về 800x600)
    // Ảnh gốc 1024x1024 được scale về 800x600, nên tỷ lệ scale ~0.78
    const float buttonWidth = 80.f;   // Chiều rộng ô
    const float buttonHeight = 60.f;  // Chiều cao ô
    const float startX = 125.f;         // Vị trí X bắt đầu
    const float row1Y = 245.f;         // Vị trí Y của hàng 1 (map 1-5)
    const float row2Y = 335.f;         // Vị trí Y của hàng 2 (map 6-10)
    const float spacingX = 115.f;      // Khoảng cách giữa các ô

    for (int i = 0; i < numMaps; ++i) {
        sf::RectangleShape btn;
        btn.setSize({ buttonWidth, buttonHeight });
        btn.setFillColor(sf::Color(60, 60, 60, 0));  // Trong suốt ban đầu
        btn.setOutlineColor(sf::Color::White);
        btn.setOutlineThickness(0.f);

        int col = i % 5;  // Cột (0-4)
        int row = i / 5;  // Hàng (0-1)

        float posX = startX + col * spacingX;
        float posY = (row == 0) ? row1Y : row2Y;

        btn.setPosition(posX, posY);

        sf::Text num;
        num.setFont(font);
        num.setString(std::to_string(i + 1));
        num.setCharacterSize(48);
        num.setFillColor(sf::Color::White);
        num.setStyle(sf::Text::Bold);

        // Căn giữa số trong ô
        sf::FloatRect textBounds = num.getLocalBounds();
        num.setOrigin(textBounds.left + textBounds.width / 2.f,
            textBounds.top + textBounds.height / 2.f);
        num.setPosition(posX + buttonWidth / 2.f, posY + buttonHeight / 2.f);

        mapButtons.push_back(btn);
        mapItems.push_back(num);
    }
}

void Menu::drawSelectionMap(sf::RenderWindow& window) {
    window.draw(selectMapBackgroundSprite);

    for (size_t i = 0; i < mapButtons.size(); ++i) {
        // Highlight map đang được chọn
        if (static_cast<int>(i) == highlightedMapIndex) {
            mapButtons[i].setFillColor(sf::Color(100, 100, 100, 220));
            mapButtons[i].setOutlineThickness(4.f);
            mapButtons[i].setOutlineColor(sf::Color::Yellow);  // Viền màu vàng
        }
        else {
            mapButtons[i].setFillColor(sf::Color(60, 60, 60, 0));
            mapButtons[i].setOutlineThickness(0.f);
        }

        window.draw(mapButtons[i]);
        window.draw(mapItems[i]);
    }

    // Hiển thị hướng dẫn - CẬP NHẬT
    sf::Text note;
    note.setFont(font);
    note.setString("Use Arrow Keys to navigate | Enter to select | ESC to return");
    note.setCharacterSize(16);
    note.setFillColor(sf::Color::White);
    note.setStyle(sf::Text::Bold);
    note.setPosition(180.f, 560.f);
    window.draw(note);
}