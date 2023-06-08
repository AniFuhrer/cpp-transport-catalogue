#pragma once

#include "geo.h"
#include "json.h"
#include "domain.h"

#include <unordered_map>
#include <string>
#include <deque>
#include <unordered_set>
#include <iostream>
#include <iomanip>
#include <vector>
#include <set>
#include <algorithm>

class TransportCatalogue{
public:

    void AddStop(const Stop& stop);

    const Stop* GetStop(const std::string& str);

    void AddBus(const Bus& bus);

    const Bus* GetBus(const std::string& str);

    std::vector<Bus> GetAllBuses();

    std::set<std::string> GetAllBusesForStop(const std::string& stop);

    void AddCoordinates(const geo::Coordinates& coordinates);

    std::vector<geo::Coordinates> GetAllCoordinates();

private:
    std::vector<geo::Coordinates> coordinates_;
    std::unordered_map<std::string, const Bus*> buses_;
    std::unordered_map<std::string, const Stop*> stops_;
    std::deque<Bus> bus_;
    std::deque<Stop> stop_;
};

class StopCommand {
public:
    explicit StopCommand(const json::Node* node);
    Stop Parse();
private:
    const json::Node* to_parse;
};

class BusCommand {
public:
    BusCommand(const json::Node* node, TransportCatalogue* catalogue);
    Bus Parse();
private:
    TransportCatalogue* catalogue_;
    const json::Node* to_parse;
};