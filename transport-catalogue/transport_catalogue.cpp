#include "transport_catalogue.h"

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

vector<Bus> TransportCatalogue::GetAllBuses() {
    vector<Bus> buses;
    for (const auto& bus : bus_) {
        buses.push_back(bus);
    }

    std::sort(buses.begin(), buses.end(), [](const Bus& lhs, const Bus& rhs) {
        return lhs.name < rhs.name;
    });
    return buses;
}

set<string> TransportCatalogue::GetAllBusesForStop(const string& stop) {
    set<std::string> buses;
    for(const auto& bus : bus_) {
        for(const auto& s : bus.unique_stops) {
            if(s->name == stop) {
                buses.insert(bus.name);
            }
        }
    }
    return buses;
}

void TransportCatalogue::AddCoordinates(const geo::Coordinates& coordinates) {
    coordinates_.push_back(coordinates);
}

std::vector<geo::Coordinates> TransportCatalogue::GetAllCoordinates() {
    return coordinates_;
}


// -----    TransportCatalogue  ----- end

StopCommand::StopCommand(const json::Node* node)
        :to_parse(node) {
}

Stop StopCommand::Parse() {
    Stop stop;
    stop.name = to_parse->AsMap().at("name").AsString();
    //cout << "Stop " + stop.name << ": " ;
    stop.coordinates.lat = to_parse->AsMap().at("latitude").AsDouble();
    stop.coordinates.lng = to_parse->AsMap().at("longitude").AsDouble();

    //cout << stop.name << ":  " <<stop.coordinates.lat << "  " << stop.coordinates.lng << endl;
    if (to_parse->AsMap().empty()){
        return stop;
    }
    for(const auto& road_distances : to_parse->AsMap().at("road_distances").AsMap()) {
        stop.distance_to_other_stops[road_distances.first] = road_distances.second.AsDouble();
    }
    return stop;
}

BusCommand::BusCommand(const json::Node* node, TransportCatalogue* catalogue)
        :to_parse(node),
         catalogue_(catalogue) {
}

//256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye
//750: Tolstopaltsevo - Marushkino - Rasskazovka

Bus BusCommand::Parse() {
    //cout << "Starting parse a bus ..." << endl;
    //cout << to_parse << endl;
    Bus bus;
    const Stop* st;
    const Stop* past_st = nullptr;
    bus.name = to_parse->AsMap().at("name").AsString();
    //cout << "First token completed ..." << "  Bus name: " + bus.name << endl;
    bool circle = to_parse->AsMap().at("is_roundtrip").AsBool();
    if (circle) {
        //cout << "Starting circle route ..." << endl;
        for (const auto& roundtrip : to_parse->AsMap().at("stops").AsArray()) {
            st = catalogue_->GetStop(roundtrip.AsString());
            bus.unique_stops.insert(st);
            bus.stops_list.push_back(st);
            bus.stops_count += 1;
            if (past_st != nullptr){
                bus.dist += ComputeDistance(past_st->coordinates, st->coordinates);
                if (past_st->distance_to_other_stops.count(st->name)) {
                    bus.route_length += past_st->distance_to_other_stops.at(st->name);
                } else if (!past_st->distance_to_other_stops.count(st->name)){
                    bus.route_length += st->distance_to_other_stops.at(past_st->name);
                }
            }
            past_st = st;
        }
    } else {
        //cout << "Starting straight route ..." << endl;
        for (const auto& stop : to_parse->AsMap().at("stops").AsArray()) {
            st = catalogue_->GetStop(stop.AsString());
            bus.unique_stops.insert(st);
            bus.stops_list.push_back(st);
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
        }
        bus.stops_count *= 2;
        --bus.stops_count;
        bus.dist *= 2;
        //    vec.insert(vec.end(), temp.begin(), temp.end());
        vector<const Stop*> temp(bus.stops_list.rbegin() + 1, bus.stops_list.rend());
        bus.stops_list.insert(bus.stops_list.end(), temp.begin(), temp.end());
        //cout << "Straight route completed ..." << endl;
    }
    bus.curvature = bus.route_length/bus.dist;
    return bus;
}