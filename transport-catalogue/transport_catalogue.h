#pragma once

#include "geo.h"

#include <unordered_map>
#include <string>
#include <deque>
#include <unordered_set>
#include <iostream>
#include <iomanip>
#include <vector>
#include <set>

namespace utils {
    std::string GetToken(const std::string& str, int& pos, char to_find);
}

struct Stop {
    std::string name;
    Coordinates coordinates;
    std::unordered_map<std::string, double> distance_to_other_stops;
};

struct Bus {
    std::string name;
    std::unordered_set<const Stop*> unique_stops;
    int stops_count = 0;
    double route_length = 0;
    double dist = 0;
    double curvature;
};

class TransportCatalogue{
public:

    void AddStop(const Stop& stop);

    const Stop* GetStop(const std::string& str);

    void AddBus(const Bus& bus);

    const Bus* GetBus(const std::string& str);

    std::set<std::string> GetAllBusesForStop(const std::string& stop);

private:
    std::unordered_map<std::string, const Bus*> buses_;
    std::unordered_map<std::string, const Stop*> stops_;
    std::deque<Bus> bus_;
    std::deque<Stop> stop_;
};

class StopCommand {
public:
    explicit StopCommand(std::string  str);
    Stop Parse();
private:
    std::string to_parse;
};

class BusCommand {
public:
    BusCommand(std::string  str, TransportCatalogue* catalogue);
    Bus Parse();
private:
    TransportCatalogue* catalogue_;
    std::string to_parse;
};