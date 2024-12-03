//
// Created by lilic on 12/2/2024.
//

#ifndef PATHSONG_SONG_H
#define PATHSONG_SONG_H

#include "../include/types.h"
#include <iostream>
using namespace std;

#include <string>
#include <vector>

class Song {
public:
    string name;
    float energy;  // from 0.0 - 1.0; measure of intensity and activity
    int key;  // standard pitch class notation; 0 = C, 1 = C#/Db, 2 = D, etc.
    int mode;  // major (1) or minor (0)
    string genre;


    // given a line from the csv file, extract the needed attributes
    void from_csv_entry(std::string data);

    // given another song, calculate the similarity
    // returns an integer from 0 to 255, where 255 is least similar
    u8 similarity(Song &rhs);
};


#endif //PATHSONG_SONG_H
