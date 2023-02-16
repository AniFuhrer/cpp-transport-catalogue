#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

using namespace std;

int main() {
    TransportCatalogue bus_catalogue;
    int operations_count_to_input = ReadLineWithNumber();
    vector<BusCommand*> bus_commands;
    vector<StopCommand*> stop_commands;

    for (int i = 0; i < operations_count_to_input; ++i){
        //input bus_/bus_stop info
        ParseLine(ReadLine(), bus_commands, stop_commands, &bus_catalogue);
    }

    for (auto stop_command : stop_commands) {
        bus_catalogue.AddStop(stop_command->Parse());
    }

    for (auto bus_command : bus_commands) {
        bus_catalogue.AddBus(bus_command->Parse());
    }

    int operations_count_to_output = ReadLineWithNumber();
    for (int i = 0; i < operations_count_to_output; ++i){
        //output bus_ info
        OutputInfo(ReadLine(), &bus_catalogue);
    }
}

