#include "Game.h"
#include <iostream>
using namespace std;
sf::Texture backgroundTexture;
sf::Sprite backgroundSprite;


Game::Game() : window(sf::VideoMode::getDesktopMode(), "Sokoban", sf::Style::Fullscreen),
currentMap(nullptr),
gameState(GameState::MENU),
showWinMessage(false) {
    window.setFramerateLimit(60);
    // === ẢNH NỀN FULL MÀN HÌNH ===
    if (!backgroundTexture.loadFromFile("D:\\PBL2\\SOKOBAN2\\SOKOBAN1\\SOKOBAN\\SOKOBAN\\images\\background.jpg")) {
        cerr << "Failed to load background image!" << endl;
    }
    backgroundSprite.setTexture(backgroundTexture);

    // Scale ảnh nền vừa khít màn hình
    sf::Vector2u textureSize = backgroundTexture.getSize();
    sf::Vector2u windowSize = window.getSize();
    backgroundSprite.setScale(
        float(windowSize.x) / textureSize.x,
        float(windowSize.y) / textureSize.y
    );


    if (!backgroundMusic.openFromFile("D:\\PBL2\\SOKOBAN2\\SOKOBAN1\\SOKOBAN\\SOKOBAN\\hi.ogg")) {
        cerr << "Failed to load background music!" << endl;
    }

    if (!moveSoundBuffer.loadFromFile("D:\\PBL2\\SOKOBAN2\\SOKOBAN1\\SOKOBAN\\SOKOBAN\\step.ogg")) {
        cerr << "Failed to load move sound!" << endl;
    }
    moveSound.setBuffer(moveSoundBuffer);

    // THÊM ÂM THANH CHIẾN THẮNG
    if (!winSoundBuffer.loadFromFile("D:\\PBL2\\SOKOBAN2\\SOKOBAN1\\SOKOBAN\\SOKOBAN\\win.ogg")) {
        cerr << "Failed to load win sound!" << endl;
    }
    winSound.setBuffer(winSoundBuffer);

    // THÊM HÌNH ẢNH CHIẾN THẮNG
    if (!winTexture.loadFromFile("D:\\PBL2\\SOKOBAN2\\SOKOBAN1\\SOKOBAN\\SOKOBAN\\images\\win.jpg")) {
        cerr << "Failed to load win message texture!" << endl;
        // Tạo texture mặc định nếu không load được ảnh
        winTexture.create(800, 600);
        sf::Uint8* pixels = new sf::Uint8[800 * 600 * 4];
        for (int i = 0; i < 800 * 600 * 4; i += 4) {
            pixels[i] = 50;      // R
            pixels[i + 1] = 80; // G  
            pixels[i + 2] = 120;   // B
            pixels[i + 3] = 255; // A
        }
        winTexture.update(pixels);
        delete[] pixels;
    }
    winSprite.setTexture(winTexture);
    winSprite.setPosition(200, 150); // Vị trí giữa màn hình

    menu.loadResources();
}

Game::~Game() {
    cleanupGame();
}

void Game::run() {
    backgroundMusic.setLoop(true);
    backgroundMusic.play();

    while (window.isOpen()) {
        handleEvents();
        update();
        render();
    }
}

void Game::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        if (gameState == GameState::MENU) {
            menu.handleEvent(event, window);
        }
        else if (gameState == GameState::PLAYING) {
            if (showWinMessage) {
                // Khi đang hiển thị thông báo chiến thắng, nhấn phím bất kỳ để quay lại menu
                if (event.type == sf::Event::KeyPressed) {
                    gameState = GameState::MENU;
                    menu.setState(MenuState::MENU);
                    cleanupGame();
                    showWinMessage = false;
                    backgroundMusic.play(); // Tiếp tục nhạc nền
                }
            }
            else {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Escape) {
                        gameState = GameState::MENU;
                        menu.setState(MenuState::MENU);
                        cleanupGame();
                    }
                    // XU LY PHIM U DE UNDO
                    else if (event.key.code == sf::Keyboard::U) {
                        if (currentMap) {
                            currentMap->undo();
                            cout << "Undo! Con lai " << currentMap->getMoveCount() << " nuoc di." << endl;
                        }
                    }
                    else if (currentMap) {
                        bool moved = false;
                        if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W) {
                            moved = currentMap->tryMovePlayer(0, -1);
                        }
                        else if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S) {
                            moved = currentMap->tryMovePlayer(0, 1);
                        }
                        else if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A) {
                            moved = currentMap->tryMovePlayer(-1, 0);
                        }
                        else if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D) {
                            moved = currentMap->tryMovePlayer(1, 0);
                        }

                        if (moved) {
                            playMoveSound();
                        }
                    }
                }
            }
        }
    }
}

void Game::playMoveSound() {
    moveSound.play();
}

void Game::update() {
    if (gameState == GameState::MENU) {
        menu.update();

        MenuState menuState = menu.getState();
        if (menuState == MenuState::PLAYING) {
            gameState = GameState::PLAYING;
            initializeGame();
            showWinMessage = false; // Reset thông báo chiến thắng
        }
        else if (menuState == MenuState::EXIT) {
            window.close();
        }
    }
    else if (gameState == GameState::PLAYING) {
        // KIỂM TRA CHIẾN THẮNG
        if (currentMap && currentMap->checkWin() && !showWinMessage) {
            showWinMessage = true;
            winSound.play(); // Phát âm thanh chiến thắng
            backgroundMusic.stop(); // Dừng nhạc nền
            cout << "Chuc mung! Ban da chien thang!" << endl;
        }
    }
}

void Game::render() {
    // Xóa màn hình
    window.clear();

    // === VẼ ẢNH NỀN FULL MÀN HÌNH ===
    window.draw(backgroundSprite);

    // === TẠO VIEW CHO GAME 800x600 GIỮA MÀN HÌNH ===
    sf::View gameView(sf::FloatRect(0, 0, 800, 600));
    sf::Vector2u screenSize = window.getSize();
    gameView.setViewport(sf::FloatRect(
        (screenSize.x - 800.f) / (2.f * screenSize.x),   // Căn giữa ngang
        (screenSize.y - 600.f) / (2.f * screenSize.y),   // Căn giữa dọc
        800.f / screenSize.x,                            // Tỷ lệ chiều ngang
        600.f / screenSize.y                             // Tỷ lệ chiều dọc
    ));
    window.setView(gameView);

    // === VẼ NỘI DUNG GAME ===
    if (gameState == GameState::MENU) {
        menu.draw(window);
    }
    else if (gameState == GameState::PLAYING) {
        if (currentMap) {
            currentMap->draw(window);

            // HIỂN THỊ THÔNG BÁO CHIẾN THẮNG
            if (showWinMessage) {
                // Vẽ lớp phủ mờ
                sf::RectangleShape overlay(sf::Vector2f(800, 600));
                overlay.setFillColor(sf::Color(0, 0, 0, 150));
                window.draw(overlay);

                // Vẽ thông báo chiến thắng
                window.draw(winSprite);

                // Thêm text hướng dẫn
                sf::Font font;
                if (font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
                    sf::Text instruction;
                    instruction.setFont(font);
                    instruction.setString("Nhan phim bat ky de quay ve Menu");
                    instruction.setCharacterSize(20);
                    instruction.setFillColor(sf::Color::White);
                    instruction.setPosition(250, 400);
                    window.draw(instruction);
                }
            }
        }
    }

    // === HIỂN THỊ LÊN MÀN HÌNH ===
    window.display();

    // Trả view về mặc định (đề phòng nếu có vẽ UI toàn màn hình sau này)
    window.setView(window.getDefaultView());
}

void Game::initializeGame() {
    cleanupGame();

    // Lấy số map được chọn từ menu (0-9 tương ứng với map 1-10)
    int selectedMap = menu.getSelectedMap();

    // Nếu chưa chọn map nào (selectedMap = -1), mặc định chọn map 1
    if (selectedMap < 0) {
        selectedMap = 0;
    }

    // Tạo tên file: map1.txt, map2.txt, ..., map10.txt
    std::string mapFileName = "map" + std::to_string(selectedMap + 1) + ".txt";

    std::cout << "Loading map: " << mapFileName << std::endl;

    // Load map đã chọn
    currentMap = new Map(mapFileName, 32);
    std::cout << "Game initialized with " << mapFileName << "!" << std::endl;
}

void Game::cleanupGame() {
    if (currentMap) {
        delete currentMap;
        currentMap = nullptr;
    }
}