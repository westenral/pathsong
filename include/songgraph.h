//
// Created by lilic on 12/2/2024.
//

#ifndef PATHSONG_SONGGRAPH_H
#define PATHSONG_SONGGRAPH_H


#include "types.h"

#include <string>
#include <vector>

class SongGraph {
private:

    // return the edge weight between the two vertices
    // edge weight is identical to similarity score (`Song::similarity`)
    u8 edge(size_t v1, size_t v2);

    std::vector<size_t> get_path(size_t song1, size_t song2);

public:

    // the primary function of the SongGraph. Given two names, finds the best
    // path between them.
    std::vector<std::string> get_path(std::string &song1, std::string &song2);
};


#endif //PATHSONG_SONGGRAPH_H
