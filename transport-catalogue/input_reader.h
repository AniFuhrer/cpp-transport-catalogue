#pragma once

#include "transport_catalogue.h"

#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

std::string ReadLine();

int ReadLineWithNumber();

void ParseLine(const std::string& line, std::vector<BusCommand*>& bus_command, std::vector<StopCommand*>& stop_command, TransportCatalogue* bus_catalogue);