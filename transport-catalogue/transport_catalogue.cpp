#include "transport_catalogue.h"

#include <utility>

using namespace std;

// -----    TransportCatalogue  ----- start

void TransportCatalogue::AddStop(const Stop& stop) {
    stop_.push_back(stop);
    stops_[stop.name] = &(stop_.back());
}

const Stop* TransportCatalogue::GetStop(const std::string& str) {
    if (stops_.count(str) == 0) {
        return nullptr;
    }
    return stops_.at(str);
}

void TransportCatalogue::AddBus(const Bus& bus) {
    //std::cout << "Trying to add a bus ..." << std::endl;
    //std::cout << bus.name << std::endl;
    bus_.push_back(bus);
    //std::cout << "Bus added to deque successful ..." << std::endl;
    buses_[bus.name] = &(bus_.back());
}

const Bus* TransportCatalogue::GetBus(const std::string& str) {
    if (buses_.count(str) == 0) {
        return nullptr;
    }
    return buses_.at(str);
}

std::set<std::string> TransportCatalogue::GetAllBusesForStop(const std::string& stop) {
    std::set<std::string> buses;
    for(const auto& bus : bus_) {
        for(const auto& s : bus.unique_stops) {
            if(s->name == stop) {
                buses.insert(bus.name);
            }
        }
    }
    return buses;
}

// -----    TransportCatalogue  ----- end

string utils::GetToken(const string& str, int& pos, char to_find){
    if (pos >= str.length()){
        return "";
    }
    int first_pos = pos;
    auto ret_pos = str.find(to_find, first_pos);
    if (ret_pos == string::npos) {
        ret_pos = str.length();
    }
    pos = ret_pos + 2;
    return str.substr(first_pos, ret_pos - first_pos);
}

//Marushkino: 55.595884, 37.209755

StopCommand::StopCommand(string  str)
    :to_parse(std::move(str)) {
}

Stop StopCommand::Parse() {
    Stop stop;
    int pos = 0;
    stop.name = utils::GetToken(to_parse, pos, ':');
    //cout << "Stop " + stop.name << ": " ;
    stop.coordinates.lat = stod (utils::GetToken(to_parse, pos, ','));
    //cout << setprecision(9) << stop.coordinates.lat << " --- " ;
    stop.coordinates.lng = stod (utils::GetToken(to_parse, pos, ','));
    //cout << setprecision(9) << stop.coordinates.lng << endl;
    //cout << stop.name << ":  " <<stop.coordinates.lat << "  " << stop.coordinates.lng << endl;
    string help = (utils::GetToken(to_parse, pos, 'm'));
    double dist = stod(help.empty() ? "0" : help);
    if (dist == 0) {
        return stop;
    }
    string s = (utils::GetToken(to_parse, pos, ',')).substr(3);
    while(!s.empty()){
        stop.distance_to_other_stops[s] = dist;
        s = (utils::GetToken(to_parse, pos, 'm')); //find distance
        if(s.empty()){
            break;
        }
        dist = stod(s);
        s = (utils::GetToken(to_parse, pos, ',')).substr(3);
    }
    return stop;
}

BusCommand::BusCommand(string  str, TransportCatalogue* catalogue)
    :to_parse(std::move(str)),
    catalogue_(catalogue) {
}

//256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye
//750: Tolstopaltsevo - Marushkino - Rasskazovka

Bus BusCommand::Parse() {
    //cout << "Starting parse a bus ..." << endl;
    //cout << to_parse << endl;
    Bus bus;
    int pos = 0;
    const Stop* st;
    const Stop* past_st = nullptr;
    bus.name = utils::GetToken(to_parse, pos, ':');
    //cout << "First token completed ..." << "  Bus name: " + bus.name << endl;
    bool circle = to_parse.find('>') != string::npos;
    //cout << pos << endl;
    if (circle) {
        //cout << "Starting circle route ..." << endl;
        string s = string (utils::GetToken(to_parse, pos, '>'));
        while (!s.empty()) {
            if(s.back() == ' '){
                s.erase(s.length()-1);
            }
            st = catalogue_->GetStop(s);
            bus.unique_stops.insert(st);
            bus.stops_count += 1;
            if (past_st != nullptr){
                bus.dist += ComputeDistance(past_st->coordinates, st->coordinates);
                if (past_st->distance_to_other_stops.count(st->name)) {
                    bus.route_length += past_st->distance_to_other_stops.at(st->name);
                } else if (!past_st->distance_to_other_stops.count(st->name)){
                    bus.route_length += st->distance_to_other_stops.at(past_st->name);
                } else {
                    cout << "MISSED" << endl;
                }
            }
            past_st = st;
            s = utils::GetToken(to_parse, pos, '>');
        }
    } else {
        //cout << "Starting straight route ..." << endl;
        string s = string (utils::GetToken(to_parse, pos, '-'));
        //cout << "First token in straight route is completed ..." << endl;
        while (!s.empty()) {
            if(s.back() == ' '){
                s.erase(s.length()-1);
            }
            st = catalogue_->GetStop(s);
            bus.unique_stops.insert(st);
            bus.stops_count += 1;
            if (past_st != nullptr){
                bus.dist += ComputeDistance(past_st->coordinates, st->coordinates);
                if (past_st->distance_to_other_stops.count(st->name) && st->distance_to_other_stops.count(past_st->name)) {
                    bus.route_length += past_st->distance_to_other_stops.at(st->name) +
                                        st->distance_to_other_stops.at(past_st->name);
                } else if (!past_st->distance_to_other_stops.count(st->name)){
                    bus.route_length += st->distance_to_other_stops.at(past_st->name)*2;
                } else if (!st->distance_to_other_stops.count(past_st->name)) {
                    bus.route_length += past_st->distance_to_other_stops.at(st->name)*2;
                }
            }
            past_st = st;
            s = utils::GetToken(to_parse, pos, '-');
        }
        bus.stops_count *= 2;
        --bus.stops_count;
        bus.dist *= 2;
        //cout << "Straight route completed ..." << endl;
    }
    bus.curvature = bus.route_length/bus.dist;
    return bus;
}
