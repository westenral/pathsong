#include "../include/songgraph.h"
#include <iostream>
#include <string>

int main(int argc, char **argv) {
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

    if (path.empty()) {
        std::cout << "No path found -- make sure song titles are spelled correctly!\n";
    }

    for (const auto &songname : path) {
        std::cout << songname << std::endl;
    }
    
    return 0;
}
