//
// Created by lilic on 12/2/2024.
//

#ifndef PATHSONG_SONGGRAPH_H
#define PATHSONG_SONGGRAPH_H


#include "song.h"
#include "../include/types.h"

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

class SongGraph {
private:

    std::vector<Song> songs;
    std::map<string, vector<u32>> genreMap;
    std::vector<vector<u32>> keyMap;
    std::vector<vector<u32>> modeMap;  // 0 minor, 1 major

    // don't write directly, use `insert_weight()`
    std::unordered_map<u32, std::unordered_map<u32, u8>> weight;

    void insert_weight(u32 from, u32 to, u8 weight);
    
    std::vector<u32> get_songs_with_key(u32 key);
    std::vector<u32> get_songs_with_genre(std::string genre);
    std::vector<u32> get_songs_with_mode(u32 mode);

    // return the edge weight between the two vertices
    // edge weight is identical to similarity score (`Song::similarity`)
    u8 edge(u32 v1, u32 v2);

    // return difference of two songs (255 - similarity)
    u8 diff(u32 s1, u32 s2);

    // return all adjacent edge weights
    std::vector<u8> edges(u32 v);

    std::vector<u32> get_path_astar(u32 song1, u32 song2);
    std::vector<u32> get_path_old(u32 song1, u32 song2);
    std::vector<u32> get_path(u32 song1, u32 song2);

    // find the song id based on its name
    // return songs.size() if song not found
    // (right now, implemented as a linear search through the songs vector)
    u32 find_song_id(std::string name);

public:

    bool use_astar = true;
    bool cli = false;

    SongGraph(std::string song_csv, bool cli);
    
    // the primary function of the SongGraph. Given two names, finds the best
    // path between them.
    std::vector<std::string> get_path(std::string &song1, std::string &song2);
};


#endif //PATHSONG_SONGGRAPH_H
