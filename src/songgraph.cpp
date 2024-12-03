//
// Created by lilic on 12/2/2024.
//

#include "../include/songgraph.h"
#include "../include/song.h"
#include <fstream>
#include <queue>
#include <stack>

namespace {
    struct Path {
        u32 weight;
        u32 from;
        u32 to;

        bool operator<(const Path &rhs) const {
            return weight < rhs.weight;
        }

        bool operator>(const Path &rhs) const {
            return weight > rhs.weight;
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
    u32 song1_id = 0, song2_id = 0;

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

    // list of distances to each vertex
    const u32 max_distance = 0xffffffff;
    std::vector<u32> dists(songs.size(), max_distance);
    dists[song1] = 0;

    // list of whether each vertex has been visited
    std::vector<bool> visited(songs.size(), false);
    visited[song1] = true;

    // predecessor of each vertex
    std::vector<u32> predecessors(songs.size(), song1);

    // the next edge to search
    std::priority_queue<Path, std::vector<Path>, std::greater<Path>> next_edges;

    auto current_vertex = song1;

    while (current_vertex != song2) {
        // list of all edge weights adjacent to current vertex
        std::vector<u8> adjacent_edges = this->edges(current_vertex);

        // push all edge weights into the priority queue
        for (u32 i = 0; i < adjacent_edges.size(); i++) {
            if (visited[i]) { continue; }
            // the "255 - weight" lets us construct a minimum spanning tree by
            // inverting the similarity score (to get a "difference" score) and
            // using that
            next_edges.push({255 - adjacent_edges[i] + dists[current_vertex], song1, i});
        }

        // choose next vertex to visit
        auto next_edge = next_edges.top();
        next_edges.pop();
        u32 next_vertex = next_edge.to;
        dists[next_vertex] = next_edge.weight;
        visited[next_vertex] = true;
        predecessors[next_vertex] = current_vertex;
        current_vertex = next_vertex;
    }

    std::vector<u32> path_reverse;
    while (current_vertex != song1) {
        path_reverse.push_back(current_vertex);
        current_vertex = predecessors[current_vertex];
    }

    // reverse path
    std::vector<u32> path;
    path.reserve(path_reverse.size());
    for (u32 id : path_reverse) {
        path.push_back(id);
    }

    return path;
}
