#include "../include/songgraph.h"
#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

int main(int argc, char **argv) {
    // Create the SFML window
    sf::RenderWindow window(sf::VideoMode(600, 400), "PathSong Finder");

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

    sf::Text song1Label("Song 1:", font, 20);
    sf::Text song2Label("Song 2:", font, 20);
    sf::Text resultText("", font, 18);
    sf::Text findPathButton("Find Path", font, 22);

    song1Label.setPosition(50, 50);
    song2Label.setPosition(50, 120);
    findPathButton.setPosition(250, 250);
    resultText.setPosition(50, 300);

    // Input fields (white rectangles to show text input areas)
    sf::RectangleShape inputBox1(sf::Vector2f(400, 30));
    sf::RectangleShape inputBox2(sf::Vector2f(400, 30));
    inputBox1.setPosition(150, 50);
    inputBox2.setPosition(150, 120);
    inputBox1.setFillColor(sf::Color::White);
    inputBox2.setFillColor(sf::Color::White);

    std::string song1 = "";
    std::string song2 = "";
    bool isSong1Active = true; // Flag to toggle between input boxes

    // Create a SongGraph instance
    SongGraph songgraph("../pop_songs.csv");

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

                    if (path.empty()) {
                        resultText.setString("No path found. Check spelling.");
                    } else {
                        std::string result = "Path: ";
                        for (const auto& song : path) {
                            result += song + " -> ";
                        }
                        result = result.substr(0, result.size() - 4);  // Remove last " -> "
                        resultText.setString(result);
                    }
                }

                // Switch focus between input boxes
                isSong1Active = inputBox1.getGlobalBounds().contains(mousePos.x, mousePos.y);
                if (!isSong1Active)
                    isSong1Active = inputBox2.getGlobalBounds().contains(mousePos.x, mousePos.y);
            }
        }

        // Render
        window.clear(sf::Color::Black);

        // Draw UI elements
        window.draw(song1Label);
        window.draw(song2Label);
        window.draw(inputBox1);
        window.draw(inputBox2);

        // Create text objects to display current input
        sf::Text input1Text(song1, font, 20);
        sf::Text input2Text(song2, font, 20);
        input1Text.setPosition(160, 50); // Positioning the text inside the box
        input2Text.setPosition(160, 120); // Positioning the text inside the box

        // Draw text for user input inside the boxes
        window.draw(input1Text);
        window.draw(input2Text);

        // Draw the "Find Path" button and result
        window.draw(findPathButton);
        window.draw(resultText);

        window.display();
    }

    // Code for command-line argument parsing (optional)
    std::string song1Cmd = song1;
    std::string song2Cmd = song2;

    // Use command-line arguments if provided
    if (argc == 3) {
        song1Cmd = argv[1];
        song2Cmd = argv[2];
    } else if (argc <= 1) {
        std::cout << "Song 1 name: ";
        std::getline(std::cin, song1Cmd);
        std::cout << "Song 2 name: ";
        std::getline(std::cin, song2Cmd);
    } else {
        std::cout << "Invalid argument count. Valid usage:\n"
                  << "pathsong\n"
                  << "pathsong <source_song_name> <target_song_name>\n";
        return 1;
    }

    // Get the path from the SongGraph
    auto path = songgraph.get_path(song1Cmd, song2Cmd);

    if (path.empty()) {
        std::cout << "No path found -- make sure song titles are spelled correctly!\n";
    } else {
        for (const auto &songname : path) {
            std::cout << songname << std::endl;
        }
    }

    /*
    // Test CSV data points
    vector<string> csv_data = {
            "0,5SuOikwiRyPMVoIQDJUgSV,Gen Hoshino,Comedy,Comedy,73,230666,False,0.676,0.461,1,-6.746,0,0.143,0.0322,1.01e-06,0.358,0.715,87.917,4,acoustic",
            "1,4qPNDBW1i3p13qLCt0Ki3A,Ben Woodward,Ghost (Acoustic),Ghost - Acoustic,55,149610,False,0.42,0.166,1,-17.235,1,0.0763,0.924,5.56e-06,0.101,0.267,77.489,4,acoustic",
            "2,1iJBSr7s7jYXzM8EGcbK5b,Ingrid Michaelson;ZAYN,To Begin Again,To Begin Again,57,210826,False,0.438,0.359,0,-9.734,1,0.0557,0.21,0.0,0.117,0.12,76.332,4,acoustic",
            "3,6lfxq3CG4xtTiEg7opyCyx,Kina Grannis,Crazy Rich Asians (Original Motion Picture Soundtrack),Can't Help Falling In Love,71,201933,False,0.266,0.0596,0,-18.515,1,0.0363,0.905,7.07e-05,0.132,0.143,181.74,3,acoustic"
    };

    // Process each CSV entry
    for (size_t i = 0; i < csv_data.size(); ++i) {
        Song song;
        song.from_csv_entry(csv_data[i]);

        // Output parsed fields
        cout << "Song ID" << i << ":" << endl;
        cout << "Name: " << song.name << endl;
        cout << "Energy: " << song.energy << endl;
        cout << "Key: " << song.key << endl;
        cout << "Mode: " << song.mode << endl;
        cout << "Genre: " << song.genre << endl;
        cout << endl;
    }
    */

//    std::string song1, song2;

    // use command line arguments as input, otherwise prompt the user
    if (argc == 3) {
        // lets narrow down your mashup math!
        // categories: genre, key, mode
        // take in user input for first one
        // anything else? ; take in user input 1 more time after this in case they want to narrow down w each category
        // string genre: w/ examples: anime, afrobeats, blues, etc. std::getline(std::cin, genre);  // would require pre populated category maps
        // std::getline(std::cin, key);
        // std::getline(std::cin, mode);

        song1 = argv[1];
        song2 = argv[2];
    } else if (argc <= 1) {
        std::cout << "Song 1 name: ";
        std::getline(std::cin, song1);

        std::cout << "Song 2 name: ";
        std::getline(std::cin, song2);
    } else {
        std::cout << "Invalid argument count. Valid usage:\n"
            << "pathsong\n"
            << "pathsong <source_song_name> <target_song_name>\n";
        return 1;
    }

    // SongGraph songgraph("larger_sample_songs.csv");
    // probably add a helper that narrows dataset down by whatever category -> use to create new csv that we can use to create songgraph
//    SongGraph songgraph("../pop_songs.csv");
//    auto path = songgraph.get_path(song1, song2);

    if (path.empty()) {
        std::cout << "No path found -- make sure song titles are spelled correctly!\n";
    }

    for (const auto &songname : path) {
        std::cout << songname << std::endl;
    }
    
    return 0;
}
