#include "../include/songgraph.h"
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

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

    dijkstraText.setFillColor(sf::Color::White);
    sf::RectangleShape dijkstraButton(sf::Vector2f(150, 50));
    dijkstraButton.setFillColor(sf::Color::Magenta);
    dijkstraButton.setPosition(125, 300);

    aStarText.setFillColor(sf::Color::White);
    sf::RectangleShape aStarButton(sf::Vector2f(150, 50));
    aStarButton.setFillColor(sf::Color::Magenta);
    aStarButton.setPosition(335, 300);

    findPathText.setFillColor(sf::Color::White);
    sf::RectangleShape findPathButton(sf::Vector2f(200, 50));
    findPathButton.setFillColor(sf::Color::Magenta);
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
                 << std::setw(2) << std::setfill('0') << i  // Ensures 2-digit formatting
                 << "_delay-0.06s.png";

        if (!texture.loadFromFile(filename.str())) {
            std::cerr << "Error loading frame: " << filename.str() << "\n";
            return -1;
        }

        gifFrames.push_back(texture);
    }

    sf::Sprite backgroundSprite;
    size_t currentFrame = 0;
    float frameTime = 0.1f; // Time per frame in seconds
    float elapsedTime = 0.f;
    sf::Clock clock;

    // Scroll position for the result text
    int scrollPos = 0;
    const int scrollStep = 30;  // Number of lines to scroll per step
    const int maxScroll = 200; // Adjust this depending on the length of your results

    // Main loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Text input handling
            if (event.type == sf::Event::TextEntered) {
                // Handle backspace to delete last character
                if (event.text.unicode == '\b') {
                    if (isSong1Active && !song1.empty())
                        song1.pop_back();
                    else if (!isSong1Active && !song2.empty())
                        song2.pop_back();
                }
                    // Handle regular characters
                else if (event.text.unicode < 128) {
                    char c = static_cast<char>(event.text.unicode);
                    if (isSong1Active)
                        song1 += c;
                    else
                        song2 += c;
                }
            }

            // Mouse interaction for the Find Path button
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                // Check if the "Find Path" button was clicked
                if (findPathButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    // Find the path between the songs
                    auto path = songgraph.get_path(song1, song2);

                    // Declare a variable to track the scroll position
                    float scrollOffset = 0.0f;
                    float scrollSpeed = 50.0f; // Adjust the speed of scrolling

                    if (path.empty()) {
                        resultText.setString("No path found. Check spelling.");
                    } else {
                        std::string result = "Path: ";
                        for (const auto& song : path) {
                            result += song + " -> ";
                        }
                        result = result.substr(0, result.size() - 4);  // Remove last " -> "

                        // Create an sf::Text object for calculating text bounds
                        sf::Text tempText(result, font, 18);

                        // Manually wrap text if it exceeds the screen width
                        sf::FloatRect bounds = tempText.getLocalBounds();
                        if (bounds.width > window.getSize().x - 100) {
                            // Wrap the text manually
                            std::string wrappedText = "";
                            std::string currentLine = "";
                            std::istringstream stream(result);
                            std::string word;

                            while (stream >> word) {
                                // Check if the current line width plus the next word exceeds the width of the window
                                if ((currentLine + " " + word).size() * 9 < window.getSize().x - 100) { // Approximate average width of characters
                                    currentLine += " " + word;
                                } else {
                                    wrappedText += currentLine + "\n";
                                    currentLine = word;
                                }
                            }
                            wrappedText += currentLine; // Add the last line

                            resultText.setString(wrappedText);
                        } else {
                            // If the text fits, just set the result text
                            resultText.setString(result);
                        }

                        // Adjust the character size and position of the result text
                        resultText.setCharacterSize(18);  // Adjust the size of the text to fit the screen

                        // Set the starting position near the center horizontally
                        float resultTextWidth = resultText.getLocalBounds().width;
                        float centerX = (window.getSize().x - resultTextWidth) / 2;

                        centerX += 450; // Shift 100 pixels to the right (use negative value for left shift)

                        resultText.setPosition(centerX, 25 - scrollOffset);  // Use the desired Y position
                        scrollOffset += scrollSpeed;

                        // If the text has scrolled too far, reset to start position
                        if (scrollOffset > resultText.getLocalBounds().height) {
                            scrollOffset = 0.0f; // Reset the scroll when it goes out of the window
                        }
                    }
                }

                // Switch focus between input boxes by increasing the clickable area
                if (inputBox1.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    isSong1Active = true;
                } else if (inputBox2.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    isSong1Active = false;
                }
            }

            // Handle scrolling
            if (event.type == sf::Event::MouseWheelScrolled) {
                if (event.mouseWheelScroll.delta > 0) {
                    scrollPos -= scrollStep;  // Scroll up
                } else if (event.mouseWheelScroll.delta < 0) {
                    scrollPos += scrollStep;  // Scroll down
                }

                // Make sure the scroll position is within bounds
                if (scrollPos < 0) {
                    scrollPos = 0;
                }
                if (scrollPos > maxScroll) {
                    scrollPos = maxScroll;
                }
            }
        }

        // Update the GIF background
        elapsedTime += clock.restart().asSeconds();
        if (elapsedTime >= frameTime) {
            elapsedTime = 0.f;
            currentFrame = (currentFrame + 1) % gifFrames.size();
            backgroundSprite.setTexture(gifFrames[currentFrame]);

            // Scale the current frame to fit the window
            float scaleX = static_cast<float>(window.getSize().x) / backgroundSprite.getTexture()->getSize().x;
            float scaleY = static_cast<float>(window.getSize().y) / backgroundSprite.getTexture()->getSize().y;
            backgroundSprite.setScale(scaleX, scaleY);
            backgroundSprite.setPosition(0, 0); // Align to top-left of the window
        }
        // Render
        window.clear();

        window.draw(backgroundSprite);
        window.draw(title);
        window.draw(song1Label);
        window.draw(song2Label);
        window.draw(inputBox1);
        window.draw(inputBox2);

        // Draw text for user input inside the boxes
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

        // Apply scroll offset to the result text
        sf::Text visibleResultText(resultText.getString(), font, 18);
        visibleResultText.setPosition(resultText.getPosition().x, resultText.getPosition().y - scrollPos);
        window.draw(visibleResultText);

        window.display();
    }

    return 0;
}
