//
// Created by lilic on 12/2/2024.
//

#ifndef PATHSONG_SONGGRAPH_H
#define PATHSONG_SONGGRAPH_H


#include <string>
#include <vector>
class SongGraph {
public:
    // the primary function of the SongGraph. Given two names, finds the best
    // path between them.
    std::vector <std::string> get_path(std::string &song_1, std::string &song_2);
};


#endif //PATHSONG_SONGGRAPH_H
