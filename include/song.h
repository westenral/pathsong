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
    unsigned int key;  // standard pitch class notation; 0 = C, 1 = C#/Db, 2 = D, etc.
    unsigned int mode;  // major (1) or minor (0)
    string genre;
    //constructor
    Song() : key(0), mode(0){};
    // given a line from the csv file, extract the needed attributes
    void from_csv_entry(std::string data);
    string get_name() const{
        return name;
    }
    string get_genre() const{
        return genre;
    }
    unsigned int get_key() const{
        return key;
    }
    unsigned int get_mode() const{
        return mode;
    }
    // given another song, calculate the similarity
    // returns an integer from 0 to 255, where 255 is most
    u8 similarity(Song &rhs);
};


#endif //PATHSONG_SONG_H
