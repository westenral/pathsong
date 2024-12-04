//
// Created by lilic on 12/2/2024.
//

#include <cmath>
#include "../include/song.h"

void Song::from_csv_entry(std::string data) {  // extracting attributes
    // indices: 4: name, 9: energy, 10: key, 12: mode, 20: genre
    vector<string> attr;  // attribute vec
    bool in_quotes = false;
    string curr_field;

    for(size_t i = 0; i < data.size(); i++) {
        char c = data[i];
        if(c == '"') {
            if(in_quotes && i + 1 < data.size() && data[i + 1] == '"') {
                curr_field += '"';
                i++;  // skipping next quote
            }
            else {
                in_quotes = !in_quotes;
            }
        }
        else if(c == ',' && !in_quotes) {
            attr.push_back(curr_field);
            curr_field.clear();
        }
        else {
            curr_field += c;
        }
    }
    attr.push_back(curr_field);  // adding last field

    if(attr.size() > 20) {  // enough fields exist
        name = attr[4];
        energy = stof(attr[9]);
        key = stoi(attr[10]);
        mode = stoi(attr[12]);
        genre = attr[20];
    }
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

    const float genre_weight  = 1.0;
    const float energy_weight = 1.0;
    const float key_weight    = 4.0;
    const float mode_weight   = 4.0;
        
    simScore = (u8)(((
        energySim  * energy_weight
        + keySim   * key_weight
        + modeSim  * mode_weight
        + genreSim * genre_weight)
        / energy_weight + key_weight
        + mode_weight + genre_weight
    ) * 255);

    return simScore;
}
