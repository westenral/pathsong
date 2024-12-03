//
// Created by lilic on 12/2/2024.
//

#include "../include/songgraph.h"
#include "../include/song.h"
#include <fstream>
#include <queue>

namespace {
    struct Edge {
        u8 weight;
        u32 from;
        u32 to;

        bool operator<(const Edge &rhs) const {
            return weight < rhs.weight;
        }
    };
}

SongGraph::SongGraph(std::string song_csv) {
    std::ifstream data(song_csv);

    std::string entry;
    
    std::getline(data, entry);

    while (!data.eof()) {
        std::getline(data, entry);
        Song song;
        song.from_csv_entry(entry);
        songs.push_back(song);
    }
}

std::vector<std::string> SongGraph::get_path(std::string &song1, std::string &song2) {
    // TODO lookup song ids from name
    u32 song1_id, song2_id;

    // traverse internal graph to find shortest path
    auto path_ids = get_path(song1_id, song2_id);

    // translate ids to names
    std::vector<std::string> path_names;
    path_names.reserve(path_ids.size());

    for (const auto id : path_ids) {
        path_names.push_back(songs[id].name);
    }

    return path_names;
}

std::vector<u32> SongGraph::get_path(u32 song1, u32 song2) {
    // priority queue of edge weights
    // whether a vertex has been visited
    // predecessors

    // pick the smallest edge
    // if we found the target - check through predecessors to get path

    std::priority_queue<Edge> edges;

    
}
