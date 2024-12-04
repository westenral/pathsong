//
// Created by lilic on 12/2/2024.
//

#include "../include/songgraph.h"
#include "../include/song.h"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <queue>
#include <set>
using namespace std;

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

    // A* path
    struct APath {
        u32 dist;
        u32 diff;
        u32 from;
        u32 to;

        bool operator<(const APath &rhs) const {
            return dist + diff < rhs.dist + rhs.diff;
        }

        bool operator>(const APath &rhs) const {
            return dist + diff > rhs.dist + rhs.diff;
        }
    };
}

SongGraph::SongGraph(std::string song_csv, bool cli) {
    this->cli = cli;

    std::ifstream data(song_csv);
    //check if file opened successfully
    if(!data.is_open()){
        cout << "unsuccessful :( " << song_csv << endl;
        return;
    }
    std::string entry;
    
    std::getline(data, entry);

    songs.reserve(114000);
    keyMap.resize(12);
    modeMap.resize(2);

    if (cli) {
        std::clog << "Loading song database..." << std::endl;
    }
    u32 i = 0;
    while (!data.eof()) {
        if (cli && (i++ % 927 == 0)) {
            std::clog << "\rLoaded " << std::setw(6) << i << " songs\r";
        }
        std::getline(data, entry);
        Song song;
        song.from_csv_entry(entry);
        songs.push_back(song);

        u32 songid = songs.size() - 1;
        genreMap[song.get_genre()].push_back(songid);
        keyMap[song.get_key()].push_back(songid);
        modeMap[song.get_mode()].push_back(songid);
    }
    
    if (cli) {
        std::clog << "\rLoaded " << std::setw(6) << --i << " songs\n";
    }
}

std::vector<std::string> SongGraph::get_path(std::string &song1, std::string &song2) {
    const char* const keylookup[] = {
        "C",
        "Db",
        "D",
        "Eb",
        "E",
        "F",
        "Gb",
        "G",
        "Ab",
        "A",
        "Bb",
        "B",
    };
    u32 song1_id = find_song_id(song1);
    if (song1_id == songs.size()) {
        std::cerr << "Song \"" << song1 << "\" not found -- check spelling\n";
        return {};
    }
    if (cli) {
      std::clog << "Found song \"" << song1 << "\" \tGenre: "
                << songs[song1_id].genre << " \tKey: "
                << keylookup[songs[song1_id].key] << " "
                << (songs[song1_id].mode ? "major" : "minor") << "\n";
    }
    u32 song2_id = find_song_id(song2);
    if (song2_id == songs.size()) {
        std::cerr << "Song \"" << song2 << "\" not found -- check spelling\n";
        return {};
    }
    if (cli) {
      std::clog << "Found song \"" << song2 << "\" \tGenre: "
                << songs[song2_id].genre << " \tKey: "
                << keylookup[songs[song2_id].key] << " "
                << (songs[song2_id].mode ? "major" : "minor") << "\n";
    }

    std::string g1 = songs[song1_id].genre;
    u32         k1 = songs[song1_id].key;
    std::string g2 = songs[song2_id].genre;
    u32         k2 = songs[song2_id].key;

    // eliminate non-common songs
    std::vector<bool> songs_common_s1(songs.size(), false);
    std::vector<bool> songs_common_s2(songs.size(), false);
    auto songs_in_g1 = get_songs_with_genre(g1);
    auto songs_in_k1 = get_songs_with_key(k1);
    auto songs_in_g2 = get_songs_with_genre(g2);
    auto songs_in_k2 = get_songs_with_key(k2);
    
    for (u32 songid : songs_in_g1) {
        songs_common_s1[songid] = true;
    }
    for (u32 songid : songs_in_k1) {
        songs_common_s1[songid] = true;
    }
    for (u32 songid : songs_in_g2) {
        songs_common_s2[songid] = true;
    }
    for (u32 songid : songs_in_k2) {
        songs_common_s2[songid] = true;
    }

    const auto songscopy = songs;
    std::vector<u32> songlookupid(songs.size(), 0xffffffff);
    songs.clear();
    songs.reserve(songs_common_s1.size());
    for (u32 i = 0; i < songs_common_s1.size(); i++) {
        if (i == song1_id || i == song2_id || songs_common_s1[i] && songs_common_s2[i]) {
            songs.push_back(songscopy[i]);
            songlookupid[i] = songs.size() - 1;
        }
    }
    songs.shrink_to_fit();
    
    if (cli) {
        std::clog << "Building graph:\nCalculating edge weights...\n";
    }

    u32 k = 1;
    for (u32 songid : songs_in_g1) {
        if (cli) {
            std::clog << "\rGroup 1 / 4\tSong " << std::setw(6) << k << " / " << std::setw(6) << songs_in_g1.size() << "          \r";
        }
        for (u32 songid2 : songs_in_g1) {
            auto newid = songlookupid[songid];
            auto newid2 = songlookupid[songid2];
            if (newid == 0xffffffff || newid2 == 0xffffffff) { continue; }
            insert_weight(newid, newid2, songs[newid].similarity(songs[newid2]));
        }
        k++;
    }
    if (cli) {
        std::clog << '\n';
    }
    k = 1;
    for (u32 songid : songs_in_k1) {
        if (cli) {
            std::clog << "\rGroup 2 / 4\tSong " << std::setw(6) << k << " / " << std::setw(6) << songs_in_k1.size() << "          \r";
        }
        for (u32 songid2 : songs_in_k1) {
            auto newid = songlookupid[songid];
            auto newid2 = songlookupid[songid2];
            if (newid == 0xffffffff || newid2 == 0xffffffff) { continue; }
            insert_weight(newid, newid2, songs[newid].similarity(songs[newid2]));
        }
        k++;
    }
    if (cli) {
        std::clog << '\n';
    }
    k = 1;
    for (u32 songid : songs_in_g2) {
        if (cli) {
            std::clog << "\rGroup 3 / 4\tSong " << std::setw(6) << k << " / " << std::setw(6) << songs_in_g2.size() << "          \r";
        }
        for (u32 songid2 : songs_in_g2) {
            auto newid = songlookupid[songid];
            auto newid2 = songlookupid[songid2];
            if (newid == 0xffffffff || newid2 == 0xffffffff) { continue; }
            insert_weight(newid, newid2, songs[newid].similarity(songs[newid2]));
        }
        k++;
    }
    if (cli) {
        std::clog << '\n';
    }
    k = 1;
    for (u32 songid : songs_in_k2) {
        if (cli) {
            std::clog << "\rGroup 4 / 4\tSong " << std::setw(6) << k << " / " << std::setw(6) << songs_in_k2.size() << "          \r";
        }
        for (u32 songid2 : songs_in_k2) {
            auto newid = songlookupid[songid];
            auto newid2 = songlookupid[songid2];
            if (newid == 0xffffffff || newid2 == 0xffffffff) { continue; }
            insert_weight(newid, newid2, songs[newid].similarity(songs[newid2]));
        }
        k++;
    }
    if (cli) {
        std::clog << '\n';
    }

    song1_id = songlookupid[song1_id];
    song2_id = songlookupid[song2_id];

    // std::cout << songs.size() << '\n';

    if (song1_id == songs.size() || song2_id == songs.size()) {
        return {};
    }

    // traverse internal graph to find shortest path
    auto path_ids = use_astar ? get_path_astar(song1_id, song2_id)
                              : get_path(song1_id, song2_id);

    // translate ids to names
    std::vector<std::string> path_names;
    path_names.reserve(path_ids.size());

    for (const auto id : path_ids) {
        path_names.push_back(songs[id].name);
    }

    return path_names;
}

void SongGraph::insert_weight(u32 from, u32 to, u8 weight) {
    this->weight[from][to] = weight;
    this->weight[to][from] = weight;
}

u8 SongGraph::diff(u32 s1, u32 s2) {
    return 255 - weight[s1][s2];
}

std::vector<u32> SongGraph::get_path_astar(u32 song1, u32 song2) {
    // all paths to consider
    // the best one is the one that is closest to start and finish
    // the weight of each edge is the distance to its source vertex plus its
    // difference from the target song
    std::priority_queue<APath, std::vector<APath>, std::less<APath>> paths;

    std::cout << songs.size() << ' ' << song1 << '\n';
    // every known distance and unsure possibly distance
    std::vector<u32> dist(songs.size(), 0xffffffff);
    dist[song1] = 0;

    // whether a vertex has been visited
    std::vector<bool> visited(songs.size(), false);
    visited[song1] = true;

    // predecessor map
    std::vector<u32> pred(songs.size(), song1);
    
    // insert each of song1's neighbors into the path queue
    for (const auto &[to, weight] : weight[song1]) {
        // get it's similarity to song2

        paths.push({diff(song1, to), diff(to, song2), song1, to});
    }

    u32 songs_checked = 0;
    while (!visited[song2]) {
        if (cli) {
            std::clog << "\rChecked " << songs_checked++ << " songs                      \r";
        }
        // take the next optimal path
        auto path = paths.top();
        paths.pop();

        // bug--optimal path not guaranteed, can't skip?
        if (visited[path.to]) { continue; }

        // visit path.to
        visited[path.to] = true;
        pred[path.to] = path.from;
        dist[path.to] = path.dist;

        // add path.to's neighbors into queue
        for (const auto &[next, weight] : weight[path.to]) {
            paths.push({path.dist + diff(path.from, path.to), diff(next, song2), path.to, next});
        }
    }
    if (cli) {
        std::clog << "\n";
    }

    // find the path from end to start
    std::vector<u32> path;
    u32 next_song_id = song2;
    while (next_song_id != song1) {
        path.push_back(next_song_id);
        next_song_id = pred[next_song_id];
    }
    path.push_back(song1);
    std::reverse(path.begin(), path.end());
    return path;
    
    /*
    std::unordered_map<u32, u8> similarities;

    std::vector<u32> path;
    path.push_back(song1);
    u32 current_song = song1;
    u32 most_similar_song_id = song1;
    u32 high_score = 0;

    // nopeFIXME currently doesn't account for difference between start song and
    // intermediate song
    do {
        similarities = weight[current_song];

        for (const auto &[songid, score] : similarities) {
            // if the next song is ranked with song2... truism?
            if (weight[songid].count(song2)) {
                if (weight[songid][song2] > high_score) {
                    most_similar_song_id = songid;
                    high_score = weight[songid][song2];
                }
            }
        }

        if (most_similar_song_id == current_song) {
            break;
        }

        path.push_back(most_similar_song_id);
        current_song = most_similar_song_id;
    } while (most_similar_song_id != current_song);

    path.push_back(song2);
    return path;
    */
}

std::vector<u32> SongGraph::get_path(u32 song1, u32 song2) {
    return get_path_old(song1, song2);

    /*
    std::vector<u32> dist(songs.size(), 0xffffffff);
    dist[song1] = 0;
    std::vector<u32> predecessor(songs.size(), song1);
    std::priority_queue<Path, std::vector<Path>, std::less<Path>> paths;
    std::vector<bool> visited(songs.size(), false);
    visited[song1] = true;

    for (const auto &[to, weight] : weight[song1]) {
        paths.push({(u32)255 - weight, song1, to});
        dist[to] = weight;
    }

    u32 i = 0;
    while (!visited[song2]) {
        std::cout << i++ << '\n';
        auto path = paths.top();
        paths.pop();

        if (visited[path.to]) { continue; }

        u32 difference = dist[path.from] + 255 - path.weight;
        
        if (dist[path.to] < difference) { continue; }

        // "visit" path.to
        visited[path.to] = true;
        dist[path.to] = difference;
        predecessor[path.to] = path.from;

        auto to_adjacent_weights = weight[path.to];
        for (const auto &[to, weight] : to_adjacent_weights) {
            // just trust me bro
            if (visited[to]) { continue; }

            u32 difference = dist[path.to] + 255 - weight;
        
            if (dist[to] < difference) { continue; }

            paths.push({difference, path.to, to});
            dist[to] = dist[path.to] + weight;
        }
    }

    std::vector<u32> path;
    u32 next_song_id = song2;
    while (next_song_id != song1) {
        path.push_back(next_song_id);
        next_song_id = predecessor[next_song_id];
    }
    path.push_back(song1);
    std::reverse(path.begin(), path.end());

    return path;
    */
}

std::vector<u32> SongGraph::get_path_old(u32 song1, u32 song2) {
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
        if (cli) {
            std::clog << "\rChecked " << steps++ << " songs\r";
        }
        // take the next edge
        auto path = paths.top();
        paths.pop();

        // if we've already visited 'to', skip
        if (visited[path.to]) { continue; }

        // we've visited 'to'
        visited[path.to] = true;

        // set dist to 'to'
        dist[path.to] = path.weight;

        predecessor[path.to] = path.from;
        
        // add all of to's edges to the queue
        weights = edges(path.to);
        for (u32 i = 0; i < weights.size(); i++) {
            // if we've visited the vertex already, skip
            if (visited[i]) { continue; }
            
            // add an edge: weight = dist[from] + diff score
            //              from = to
            //              to = next vtex
            paths.push({dist[path.from] + 255 - weights[i], path.to, i});
        }
    }
    if (cli) {
        std::clog << '\n';
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
        if (song.name == name) {
            // if (cli) {
                // std::clog << "this one? " << song.name << " genre " << song.genre << '\n';
                // char what;
                // std::cin >> what;
                // if (what == 'n') { continue;} else { return i; }
            // } else {
                return i;
            // }
        }
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
