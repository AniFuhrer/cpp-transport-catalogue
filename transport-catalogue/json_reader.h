#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "domain.h"
#include "request_handler.h"
#include "map_renderer.h"

#include <iostream>
#include <fstream>
#include <sstream>

json::Document LoadJSON(const std::string& s);

std::string Print(const json::Node& node);

void FillCatalogue(std::vector<StopCommand*>& stop_commands,
                   std::vector<BusCommand*>& bus_command, TransportCatalogue* catalogue);

std::pair<std::vector<json::Node>, json::Node> JsonInputStream(std::vector<StopCommand*>& stop_commands,
                     std::vector<BusCommand*>& bus_commands, TransportCatalogue* catalogue, std::istream& in);
