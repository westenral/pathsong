#include "../include/songgraph.h"
#include <iostream>
#include <string>

int main(int argc, char **argv) {
    std::string song1, song2;

    // use command line arguments as input, otherwise prompt the user
    if (argc == 3) {
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

    SongGraph songgraph("pop_songs.csv");
    auto path = songgraph.get_path(song1, song2);

    for (const auto &songname : path) {
        std::cout << songname << std::endl;
    }
    
    return 0;
}
