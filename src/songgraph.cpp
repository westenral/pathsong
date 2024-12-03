//
// Created by lilic on 12/2/2024.
//

#include "../include/songgraph.h"
#include "../include/song.h"
#include <algorithm>
#include <fstream>
#include <queue>

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
    u32 song1_id = find_song_id(song1);
    u32 song2_id = find_song_id(song2);

    if (song1_id == songs.size() || song2_id == songs.size()) {
        return {};
    }

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
    // this method works by traversing a graph where each edge weight
    // represents how different the two songs are

    // distance from origin (song1) to each vertex
    // distance to origin is 0
    std::vector<u32> dist(songs.size(), 0xffffffff);
    dist[song1] = 0;

    // predecessor of each vertex
    std::vector<u32> predecessor(songs.size(), song1);
    
    // priority queue of accessible edges, where the next edge to travel to is
    // the edge with least weight.
    // the weights include the distance to the the vertex they are connected to
    // just because an edge is in the queue doesn't mean it's optimal, but the
    // optimal path should always be chosen first
    std::priority_queue<Path, std::vector<Path>, std::less<Path>> paths;

    // whether each vertex has been visited. a vertex is "visited" once a
    // optimal path to it has been found
    // so far, the only visited vertex is the origin
    std::vector<bool> visited(songs.size(), false);
    visited[song1] = true;

    // every time we visit a vertex, add its edges to the queue -- start off
    // with the origin
    // see below for reasoning behind formula
    std::vector<u8> weights = edges(song1);
    for (u32 i = 0; i < weights.size(); i++) {
        paths.push({(u32)255 - weights[i], song1, i});
    }

    u32 steps = 0;
    // as long as we haven't visited song2 yet, keep searching
    while (!visited[song2]) {
        std::cout << steps++ << '\n';
        // take the next edge
        auto path = paths.top();
        paths.pop();

        // if we've already visited 'to', skip
        if (visited[path.to]) { continue; }

        // we've visited 'to'
        visited[path.to] = true;

        // set dist to 'to'
        dist[path.to] = path.weight;
        
        // add all of to's edges to the queue
        weights = edges(path.to);
        for (u32 i = 0; i < weights.size(); i++) {
            // if we've visited the vertex already, skip
            if (visited[i]) { continue; }
            
            // add an edge: weight = dist[to] + diff score
            //              from = to
            //              to = next vtex
            paths.push({dist[path.to] + 255 - weights[i], path.to, i});
        }
    }

    // find the path from end to start
    std::vector<u32> path;
    u32 next_song_id = song2;
    while (next_song_id != song1) {
        path.push_back(next_song_id);
        next_song_id = predecessor[next_song_id];
    }
    path.push_back(song1);
    std::reverse(path.begin(), path.end());

    return path;
}

std::vector<u8> SongGraph::edges(u32 v) {
    // right now, it computes the edges on-the-fly
    std::vector<u8> weights;
    weights.reserve(songs.size());

    for (u32 i = 0; i < songs.size(); i++) {
        weights.push_back(edge(v, i));
    }

    return weights;
}

u8 SongGraph::edge(u32 v1, u32 v2) {
    return songs[v1].similarity(songs[v2]);
}

u32 SongGraph::find_song_id(std::string name) {
    u32 i = 0;
    for (const auto &song : songs) {
        if (song.name == name) return i;
        i++;
    }
    return i;
}

std::vector<u32> SongGraph::get_songs_with_key(u32 key) {  // using vector<vector<>>  // input is 0-11
     return keyMap[key];
}

std::vector<u32> SongGraph::get_songs_with_genre(std::string genre) {  // using map
    return genreMap[genre];
}

std::vector<u32> SongGraph::get_songs_with_mode(u32 mode) {  // using vector<vector<>>
    return modeMap[mode];
}
