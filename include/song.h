//
// Created by lilic on 12/2/2024.
//

#ifndef PATHSONG_SONG_H
#define PATHSONG_SONG_H

#include "../include/types.h"

#include <string>

class Song {
public:
    // given a line from the csv file, extract the needed attributes
    void from_csv_entry(std::string data);

    // given another song, calculate the similarity
    // returns an integer from 0 to 255, where 255 is least similar
    u8 similarity(Song &rhs);
};


#endif //PATHSONG_SONG_H
