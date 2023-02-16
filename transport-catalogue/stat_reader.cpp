#include "stat_reader.h"

using namespace std;

void PrintBus(const string& str, TransportCatalogue* catalogue){
    const Bus* bus = catalogue->GetBus(str);
    if(bus != nullptr){
        cout << setprecision (6)
        << "Bus " + bus->name + ": "
        << bus->stops_count << " stops on route, "
        << bus->unique_stops.size() << " unique stops, "
        << bus->route_length << " route length, "
        << bus->curvature << " curvature"
        << endl;
    } else {
        cout << "Bus " + str + ": not found" << endl;
    }
}

void PrintStop(const string& str, TransportCatalogue* catalogue){
    const Stop* stop = catalogue->GetStop(str);
    if(stop == nullptr) {
        cout << "Stop " + str + ": not found" << endl;
    } else {
        const set<string> buses = catalogue->GetAllBusesForStop(str);
        if(buses.empty()) {
            cout << "Stop " + str + ": no buses" << endl;
        } else {
            cout << "Stop " + str + ": buses ";
            for(const auto& bus : buses){
                cout << bus + " ";
            }
            cout << endl;
        }
    }
}

void OutputInfo(const string& output, TransportCatalogue* catalogue){
    auto it = output.find(' ');
    string cmd_type = (output.substr(0, it));
    std::transform(cmd_type.begin(), cmd_type.end(), cmd_type.begin(),
                   [](char c){return std::tolower(c);} );
    //std::transform(cmd_type.begin(), cmd_type.end(), cmd_type.begin(), std::tolower);
    if (cmd_type == "stop") {
        PrintStop((output.substr(it+1)), catalogue);
    } else if (cmd_type == "bus") {
        PrintBus((output.substr(it+1)), catalogue);
    }
}