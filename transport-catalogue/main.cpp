#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

using namespace std;

int main() {
    TransportCatalogue bus_catalogue;
    vector<BusCommand*> bus_commands;
    vector<StopCommand*> stop_commands;

    InputCatalogue(ReadLineWithNumber(cin), bus_commands, stop_commands, &bus_catalogue, cin);

    for (auto stop_command : stop_commands) {
        bus_catalogue.AddStop(stop_command->Parse());
    }

    for (auto bus_command : bus_commands) {
        bus_catalogue.AddBus(bus_command->Parse());
    }

    OutputCatalogue(ReadLineWithNumber(cin), &bus_catalogue, cout, cin);
}

