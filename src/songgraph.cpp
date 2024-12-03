//
// Created by lilic on 12/2/2024.
//

#include "../include/songgraph.h"

std::vector<std::string> SongGraph::get_path(std::string &song1, std::string &song2) {
    // lookup song ids from name
    // traverse internal graph to find shortest path
}

std::vector<size_t> SongGraph::get_path(size_t song1, size_t song2) {
    // priority queue of edge weights
    // whether a vertex has been visited
    // predecessors

    // pick the smallest edge
    // if we found the target - check through predecessors to get path
}
