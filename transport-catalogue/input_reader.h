#pragma once

#include "transport_catalogue.h"

#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

std::string ReadLine(std::istream& in);

int ReadLineWithNumber(std::istream& in);

void InputCatalogue(int operations_count_to_input, std::vector<BusCommand*>& bus_commands, std::vector<StopCommand*>& stop_commands, TransportCatalogue* bus_catalogue, std::istream& in);

void ParseLine(const std::string& line, std::vector<BusCommand*>& bus_command, std::vector<StopCommand*>& stop_command, TransportCatalogue* bus_catalogue);