#include "stat_reader.h"

using namespace std;

void PrintBus(const string& str, TransportCatalogue* catalogue, ostream& out){
    const Bus* bus = catalogue->GetBus(str);
    if(bus != nullptr){
        out << setprecision (6)
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

void PrintStop(const string& str, TransportCatalogue* catalogue, ostream& out){
    const Stop* stop = catalogue->GetStop(str);
    if(stop == nullptr) {
        out << "Stop " + str + ": not found" << endl;
    } else {
        const set<string> buses = catalogue->GetAllBusesForStop(str);
        if(buses.empty()) {
            out << "Stop " + str + ": no buses" << endl;
        } else {
            out << "Stop " + str + ": buses ";
            for(const auto& bus : buses){
                out << bus + " ";
            }
            out << endl;
        }
    }
}

void OutputInfo(const string& output, TransportCatalogue* catalogue, ostream& out){
    auto it = output.find(' ');
    string cmd_type = (output.substr(0, it));
    std::transform(cmd_type.begin(), cmd_type.end(), cmd_type.begin(),
                   [](char c){return std::tolower(c);} );
    //std::transform(cmd_type.begin(), cmd_type.end(), cmd_type.begin(), std::tolower);
    if (cmd_type == "stop") {
        PrintStop((output.substr(it+1)), catalogue, out);
    } else if (cmd_type == "bus") {
        PrintBus((output.substr(it+1)), catalogue, out);
    }
}

void OutputCatalogue(int operations_count_to_output, TransportCatalogue* catalogue, ostream& out, istream& in){
    for (int i = 0; i < operations_count_to_output; ++i){
        //output bus_ info
        OutputInfo(ReadLine(in), catalogue, out);
    }
}