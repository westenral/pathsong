//
// Created by lilic on 12/2/2024.
//

#include <sstream>
#include <cmath>
#include "../include/song.h"

void Song::from_csv_entry(std::string data) {  // extracting attributes
    // indices: 4: name, 9: energy, 10: key, 12: mode, 20: genre
    vector<string> attr;  // attribute vec
    stringstream ss(data);
    string s;  // stores token obtained from original string

    // FIXME currently ignore quotes escaping commas (example: eddie vedder "Eat, Pray, Love" splits on commas when it shouldn't)
    while(getline(ss, s, ',')) {
        attr.push_back(s);
    }
    name = attr[4];
    energy = stof(attr[9]);
    key = stoi(attr[10]);
    mode = stoi(attr[12]);
    genre = attr[20];
}

u8 Song::similarity(Song &rhs) {  // similarity sum / total similarity = similarity score
    u8 simScore;
    float energySim = 1.0 - abs(energy - rhs.energy);  // energy similarity = 1.0 - abs(e1 - e2)
    float keySim;
    float modeSim;
    float genreSim;

    float k1 = min(key, rhs.key);
    float k2 = max(key, rhs.key);
    keySim = 1.0 - (min(k2 - k1, (k1 + 12) - k2) / 6.0);  // key similarity: if k1 < k2: 1.0 - (min(k2 - k1, (k1 + 12) - k2) / 6.0)

    modeSim = mode == rhs.mode;  // bool tht returns 0 or 1
    genreSim = genre == rhs.genre;
    simScore = (u8)(((energySim + keySim + modeSim + genreSim) / 4.0) * 255);

    return simScore;
}
