#include "input_reader.h"

using namespace std;

string ReadLine(istream& in) {
    string s;
    getline(in, s);
    return s;
}

int ReadLineWithNumber(istream& in) {
    int result;
    in >> result;
    ReadLine(cin);
    return result;
}

void InputCatalogue(int operations_count_to_input, vector<BusCommand*>& bus_commands, vector<StopCommand*>& stop_commands, TransportCatalogue* bus_catalogue, istream& in){
    for (int i = 0; i < operations_count_to_input; ++i){
        //input bus_/bus_stop info
        ParseLine(ReadLine(in), bus_commands, stop_commands, bus_catalogue);
    }
}

//Stop Tolstopaltsevo: 55.611087, 37.20829, 3900m to Marushkino
//Stop Marushkino: 55.595884, 37.209755, 9900m to Rasskazovka, 100m to Marushkino
//Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye
//Bus 750: Tolstopaltsevo - Marushkino - Marushkino - Rasskazovka

void ParseLine(const string& line, vector<BusCommand*>& bus_commands, vector<StopCommand*>& stop_commands,  TransportCatalogue* bus_catalogue) {
    auto it = line.find(' ');
    string cmd_type = (line.substr(0, it));
    std::transform(cmd_type.begin(), cmd_type.end(), cmd_type.begin(),
                   [](char c){return std::tolower(c);} );
    //std::transform(cmd_type.begin(), cmd_type.end(), cmd_type.begin(), std::tolower);
    if (cmd_type == "stop") {
        stop_commands.push_back(new StopCommand(line.substr(it+1)));
    } else if (cmd_type == "bus") {
        bus_commands.push_back(new BusCommand(line.substr(it+1), bus_catalogue) );
    }
}