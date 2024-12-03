//
// Created by lilic on 12/2/2024.
//

#ifndef PATHSONG_SONGGRAPH_H
#define PATHSONG_SONGGRAPH_H


#include "song.h"
#include "../include/types.h"

#include <string>
#include <vector>

class SongGraph {
private:

    std::vector<Song> songs;

    // return the edge weight between the two vertices
    // edge weight is identical to similarity score (`Song::similarity`)
    u8 edge(u32 v1, u32 v2);

    // return all adjacent edge weights
    std::vector<u8> edges(u32 v);

    std::vector<u32> get_path(u32 song1, u32 song2);

    // find the song id based on its name
    // return -1 if song not found
    // (right now, implemented as a linear search through the songs vector)
    u32 find_song_id(std::string name);

public:

    SongGraph(std::string song_csv);

    // the primary function of the SongGraph. Given two names, finds the best
    // path between them.
    std::vector<std::string> get_path(std::string &song1, std::string &song2);
};


#endif //PATHSONG_SONGGRAPH_H
