#include "transport_catalogue.h"
#include "json.h"
#include "json_reader.h"

#include <iostream>
#include <string>

using namespace std;

string ColorToSvg(const vector<int>& color, double opacity = -1.0) {
    if (opacity < 0.0) {
        // RGB color
        return "rgb(" + to_string(color[0]) + "," + to_string(color[1]) + "," + to_string(color[2]) + ")";
    } else {
        // RGBA color
        stringstream ss;
        ss << opacity;
        return "rgba(" + to_string(color[0]) + "," + to_string(color[1]) + "," + to_string(color[2]) + "," + ss.str() + ")";
    }
}

svg::Document DocInput(const vector<Bus>& buses, const json::Node& render_settings, SphereProjector proj) {
    using namespace svg;
    Document doc;
    int i = 0;
    for(const auto& bus : buses) {
        Polyline bus_line;
        bus_line.SetStrokeWidth(render_settings.AsMap().at("line_width").AsDouble())
                .SetFillColor("none")
                .SetStrokeLineCap(StrokeLineCap::ROUND)
                .SetStrokeLineJoin(StrokeLineJoin::ROUND);
        Circle stop_circle;
        for(const auto& stop : bus.stops_list) {
            bus_line.AddPoint(proj(stop->coordinates));
            const auto& color_variant = render_settings.AsMap().at("color_palette").AsArray()[i];
            if (color_variant.IsString()) {
                bus_line.SetStrokeColor(color_variant.AsString());
            } else if (color_variant.IsArray()){
                vector<int> temp;
                double opacity = -1.0;
                auto color_array = color_variant.AsArray();
                for (size_t j = 0; j < color_array.size(); j++) {
                    if (j < 3) { // RGB values
                        temp.push_back(color_array[j].AsInt());
                    } else { // Alpha value
                        opacity = color_array[j].AsDouble();
                    }
                }
                bus_line.SetStrokeColor(ColorToSvg(temp, opacity));
            }
        }
        if(i == render_settings.AsMap().at("color_palette").AsArray().size() - 1) {
            i = 0;
        } else {
            i++;
        }
        doc.Add(bus_line);
    }
    return doc;
}

int main() {

    TransportCatalogue bus_catalogue;

    vector<BusCommand*> bus_commands;
    vector<StopCommand*> stop_commands;


    pair<vector<json::Node>, json::Node> results = JsonInputStream(stop_commands, bus_commands, &bus_catalogue, cin);

    json::Node result = results.first;
    json::Node render_settings = results.second;

    vector<Bus> buses = bus_catalogue.GetAllBuses();
    vector<geo::Coordinates> coordinates;

    for (const auto& bus : buses) {
        for(const auto& stop : bus.stops_list) {
            coordinates.push_back(stop->coordinates);
        }
    }
/*
    for (const auto geo_coord : bus_catalogue.GetAllCoordinates()) {
        const svg::Point screen_coord = proj(geo_coord);
        cout << '(' << geo_coord.lat << ", "sv << geo_coord.lng << ") -> "sv;
        cout << '(' << screen_coord.x << ", "sv << screen_coord.y << ')' << endl;
    }


    for (const auto& bus : buses) {
        cout << "Bus "sv << bus.name << ": "sv;
        for (const auto& stop : bus.stops_list) {
            cout << stop->name << "("sv << stop->coordinates.lat << ", "sv << stop->coordinates.lng << "), "sv;
        }
        cout << endl;
    }
*/

    if (!render_settings.AsMap().empty()) {

        SphereProjector proj{coordinates.begin(),
                             coordinates.end(),
                             render_settings.AsMap().at("width").AsDouble(),
                             render_settings.AsMap().at("height").AsDouble(),
                             render_settings.AsMap().at("padding").AsDouble()};
        svg::Document doc = DocInput(buses, render_settings, proj);

        doc.Render(cout);
    }
    /*
    if(!result.AsArray().empty()) {
        cout << Print(result);
    }
     */
    return 0;
}

