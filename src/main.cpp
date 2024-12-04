#include "../include/songgraph.h"
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <thread>
#include <atomic>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

std::atomic<size_t> currentFrame(0); // Shared frame index
std::atomic<bool> isRunning(true);  // Control flag for the background thread

// Function to update the background animation
void animateBackground(const std::vector<sf::Texture>& gifFrames, float frameTime) {
    sf::Clock frameClock;

    while (isRunning) {
        if (frameClock.getElapsedTime().asSeconds() >= frameTime) {
            frameClock.restart();
            currentFrame = (currentFrame + 1) % gifFrames.size(); // Loop through frames
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Prevent high CPU usage
    }
}

// Helper function for wrapping text within bounds
std::string wrapText(const std::string& text, sf::Font& font, unsigned characterSize, float maxWidth) {
    std::istringstream wordStream(text);
    std::string word, result, currentLine;
    float spaceWidth = font.getGlyph(' ', characterSize, false).advance;

    while (wordStream >> word) {
        float lineWidth = font.getGlyph(currentLine[0], characterSize, false).advance * currentLine.size();
        float wordWidth = font.getGlyph(word[0], characterSize, false).advance * word.size();

        if (lineWidth + wordWidth + spaceWidth > maxWidth) {
            result += currentLine + '\n';
            currentLine.clear();
        }
        if (!currentLine.empty()) {
            currentLine += ' ';
        }
        currentLine += word;
    }

    result += currentLine; // Add the last line
    return result;
}

int main(int argc, char **argv) {
    // check for command line interface
    if (argc >= 4 && std::string(argv[1]) == "-cli") {

        // command line functionality
        std::string song1, song2;

        // use command line arguments as input, otherwise prompt the user
        song1 = argv[2];
        song2 = argv[3];

        // SongGraph songgraph("larger_sample_songs.csv", true);
        SongGraph songgraph("pop_songs.csv", true);
        if (argc == 5 && std::string(argv[4]) == "-d") {
            songgraph.use_astar = false;
        }
        auto path = songgraph.get_path(song1, song2);
        if (path.empty()) {
            std::cerr << "No path found\n";
        }

        for (const auto &songname : path) {
            std::cout << songname << std::endl;
        }

        return 0;
    }

    // Create the SFML window
    sf::RenderWindow window(sf::VideoMode(1900, 900), "PathSong Finder");

    // Load the icon image file (e.g., "icon.png")
    sf::Image icon;
    if (!icon.loadFromFile("../include/tongueicon.png")) {
        std::cerr << "Error loading icon image\n";
        return -1;
    }

    // Set the window icon using the loaded image
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    // Fonts and Texts
    sf::Font font;
    if (!font.loadFromFile("../include/RobotoLight.ttf")) {
        std::cerr << "Error loading font\n";
        return -1;
    }

    sf::Text title("Welcome to PathSong", font, 30);
    sf::Text song1Label("Song 1:", font, 20);
    sf::Text song2Label("Song 2:", font, 20);
    sf::Text resultText("", font, 18);
    sf::Text dijkstraText("Dijkstra's", font, 24);
    sf::Text aStarText("A*", font, 24);
    sf::Text findPathText("Find Path!", font, 24);

    title.setPosition(150, 50);
    song1Label.setPosition(50, 150);
    song2Label.setPosition(50, 200);
    dijkstraText.setPosition(150, 305);
    aStarText.setPosition(400, 305);
    findPathText.setPosition(250, 400);

    sf::RectangleShape dijkstraButton(sf::Vector2f(150, 50));
    sf::RectangleShape aStarButton(sf::Vector2f(150, 50));
    sf::RectangleShape findPathButton(sf::Vector2f(200, 50));

    dijkstraButton.setFillColor(sf::Color::Magenta);
    aStarButton.setFillColor(sf::Color::Magenta);
    findPathButton.setFillColor(sf::Color::Magenta);

    dijkstraButton.setPosition(125, 300);
    aStarButton.setPosition(335, 300);
    findPathButton.setPosition(200, 395);

    // Input fields (white rectangles to show text input areas)
    sf::RectangleShape inputBox1(sf::Vector2f(400, 30));
    sf::RectangleShape inputBox2(sf::Vector2f(400, 30));
    inputBox1.setPosition(150, 150);
    inputBox2.setPosition(150, 200);
    inputBox1.setFillColor(sf::Color::White);
    inputBox2.setFillColor(sf::Color::White);

    std::string song1 = "";
    std::string song2 = "";
    bool isSong1Active = true; // Flag to toggle between input boxes

    // Create a SongGraph instance
    SongGraph songgraph("../pop_songs.csv", false);

    // Load GIF frames (pre-extracted as PNG files)
    std::vector<sf::Texture> gifFrames;
    for (int i = 0; i < 46; ++i) {  // Loop from frame_00 to frame_45
        sf::Texture texture;
        std::ostringstream filename;
        filename << "../include/discoballGIF/frame_"
                 << std::setw(2) << std::setfill('0') << i
                 << "_delay-0.06s.png";

        if (!texture.loadFromFile(filename.str())) {
            std::cerr << "Error loading frame: " << filename.str() << "\n";
            return -1;
        }
        gifFrames.push_back(texture);
    }

    // Start the animation thread
    std::thread backgroundThread(animateBackground, std::ref(gifFrames), 0.06f);

    sf::Sprite backgroundSprite;
    sf::Clock clock;

    int scrollPos = 0;
    const int scrollStep = 20;
    const int maxScroll = 200;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                isRunning = false;
                backgroundThread.join();
                window.close();
            }

            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b') {
                    if (isSong1Active && !song1.empty())
                        song1.pop_back();
                    else if (!isSong1Active && !song2.empty())
                        song2.pop_back();
                } else if (event.text.unicode < 128) {
                    char c = static_cast<char>(event.text.unicode);
                    if (isSong1Active)
                        song1 += c;
                    else
                        song2 += c;
                }
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (findPathButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    auto path = songgraph.get_path(song1, song2);
                    if (path.empty()) {
                        resultText.setString("No path found. Check spelling.");
                    } else {
                        std::string result = "Path: ";
                        for (const auto& song : path) {
                            result += song + " -> ";
                        }
                        result = result.substr(0, result.size() - 4); // Remove trailing arrow
                        result = wrapText(result, font, 18, window.getSize().x - 100); // Wrap text to fit window
                        resultText.setString(result);
                    }
                }

                if (inputBox1.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    isSong1Active = true;
                } else if (inputBox2.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    isSong1Active = false;
                }
            }

            if (event.type == sf::Event::MouseWheelScrolled) {
                float maxScroll = std::max(0.f, resultText.getLocalBounds().height - (window.getSize().y / 2));
                scrollPos = std::clamp(scrollPos + (event.mouseWheelScroll.delta > 0 ? -scrollStep : scrollStep), 0, static_cast<int>(maxScroll));

            }
        }

        window.clear();

        backgroundSprite.setTexture(gifFrames[currentFrame]);
        float scaleX = static_cast<float>(window.getSize().x) / backgroundSprite.getTexture()->getSize().x;
        float scaleY = static_cast<float>(window.getSize().y) / backgroundSprite.getTexture()->getSize().y;
        backgroundSprite.setScale(scaleX, scaleY);
        window.draw(backgroundSprite);

        window.draw(title);
        window.draw(song1Label);
        window.draw(song2Label);
        window.draw(inputBox1);
        window.draw(inputBox2);

        sf::Text input1Text(song1, font, 20);
        sf::Text input2Text(song2, font, 20);
        input1Text.setFillColor(sf::Color::Black);
        input2Text.setFillColor(sf::Color::Black);
        input1Text.setPosition(inputBox1.getPosition().x + 5, inputBox1.getPosition().y + 5);
        input2Text.setPosition(inputBox2.getPosition().x + 5, inputBox2.getPosition().y + 5);

        window.draw(input1Text);
        window.draw(input2Text);

        window.draw(dijkstraButton);
        window.draw(dijkstraText);
        window.draw(aStarButton);
        window.draw(aStarText);
        window.draw(findPathButton);
        window.draw(findPathText);

        resultText.setPosition(570, 25 - scrollPos);               // Center vertically with scroll

        window.draw(resultText);

        window.display();
    }

    return 0;
}
