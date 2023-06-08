#include "json_reader.h"


using namespace std;
using namespace json;

Document LoadJSON(const string& s) {
    istringstream strm(s);
    return Load(strm);
}

std::string Print(const Node& node) {
    ostringstream out;
    Print(Document{node}, out);
    return out.str();
}
/*
svg::Text ParseText(const Node& node) {
    svg::Text template_text =
            svg::Text()
            .SetFontSize(node.AsMap().at("bus_label_font_size").AsInt());
}

svg::Polyline ParsePolyline(const Node& node) {
    svg::Polyline template_polyline =
            svg::Polyline()
            .SetStrokeWidth(node.AsMap().at("line_width").AsInt());
}
*/
void FillCatalogue(vector<StopCommand*>& stop_commands,
                   vector<BusCommand*>& bus_commands, TransportCatalogue* catalogue) {
    for (const auto& stop_command : stop_commands) {
        catalogue->AddStop(stop_command->Parse());
        catalogue->AddCoordinates(stop_command->Parse().coordinates);
    }

    for (const auto& bus_command : bus_commands) {
        catalogue->AddBus(bus_command->Parse());
    }


}

pair<vector<Node>, Node> JsonInputStream(vector<StopCommand*>& stop_commands,
                      vector<BusCommand*>& bus_commands, TransportCatalogue* catalogue, istream& in) {
    std::stringstream buffer;
    buffer << in.rdbuf();
    Document doc = LoadJSON(buffer.str());
    vector<Node> array_results;
    Node render_settings{Dict{}};
    if(!doc.GetRoot().AsMap().at("render_settings").AsMap().empty()) {
        render_settings = {doc.GetRoot().AsMap().at("render_settings").AsMap()};
    }
    for (const auto& base_command : doc.GetRoot().AsMap()) {
        if (base_command.first == "base_requests") {
            //cout << "Loading base requests..." << endl;
            for (const auto& base_request : base_command.second.AsArray()) {
                if (base_request.AsMap().at("type") == "Bus") {
                    bus_commands.push_back(new BusCommand(&base_request, catalogue));
                } else {
                    stop_commands.push_back(new StopCommand(&base_request));
                }
            }
        } else if (base_command.first == "stat_requests") {
            //cout << "Converting stat requests..." << endl;
            FillCatalogue(stop_commands, bus_commands, catalogue);
            for (const auto& stat_request : base_command.second.AsArray()) {
                if(stat_request.AsMap().at("type").AsString() == "Bus") {
                    const Bus* bus = catalogue->GetBus(stat_request.AsMap().at("name").AsString());
                    if(bus) {
                        Node bus_node{Dict{{"curvature",         bus->curvature},
                                           {"request_id",        stat_request.AsMap().at("id").AsInt()},
                                           {"route_length",      bus->route_length},
                                           {"stop_count",        bus->stops_count},
                                           {"unique_stop_count", int(bus->unique_stops.size())}
                        }};
                        array_results.push_back(bus_node);
                    } else {
                        Node bus_node{Dict{{"request_id", stat_request.AsMap().at("id").AsInt()},
                                           {"error_message", "not found"}
                        }};
                        array_results.push_back(bus_node);
                    }
                } else if(stat_request.AsMap().at("type").AsString() == "Stop") {
                    const Stop* stop = catalogue->GetStop(stat_request.AsMap().at("name").AsString());
                    if(stop) {
                        vector<Node> buses_for_stop;
                        for (const auto &buses: catalogue->GetAllBusesForStop(stop->name)) {
                            Node bus = buses;
                            buses_for_stop.push_back(bus);
                        }
                        Node stop_node = {Dict{{"request_id", stat_request.AsMap().at("id").AsInt()},
                                               {"buses",      buses_for_stop}}};
                        array_results.push_back(stop_node);
                    } else {
                        Node stop_node{Dict{{"request_id", stat_request.AsMap().at("id").AsInt()},
                                           {"error_message", "not found"}
                        }};
                        array_results.push_back(stop_node);
                    }
                } else {
                    //cout << "Something went wrong with stat request: " + stat_request.AsMap().at("type").AsString() << endl;
                }
            }
        } else if (base_command.first == "render_settings") {
            // do  nothing
        }
        else {
            //cerr << "Unknown command: " << base_command.first << endl;
        }
    }
    return {array_results, render_settings};
}