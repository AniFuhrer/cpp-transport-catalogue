#pragma once

#include "geo.h"

#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <vector>

namespace utils {
    std::string GetToken(const std::string& str, int& pos, char to_find);
}

struct Stop {
    std::string name;
    geo::Coordinates coordinates;
    std::unordered_map<std::string, double> distance_to_other_stops;
};

struct Bus {
    std::string name;
    std::unordered_set<const Stop*> unique_stops;
    std::vector<const Stop*> stops_list;
    int stops_count = 0;
    double route_length = 0;
    double dist = 0;
    double curvature;
};

