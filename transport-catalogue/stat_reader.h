#pragma once

#include "transport_catalogue.h"
#include "input_reader.h"

#include <string>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <set>

void PrintBus(const std::string& str, TransportCatalogue* catalogue, std::ostream& out);

void PrintStop(const std::string& str, TransportCatalogue* catalogue, std::ostream& out);

void OutputInfo(const std::string& output, TransportCatalogue* catalogue, std::ostream& out);

void OutputCatalogue(int operations_count_to_output, TransportCatalogue* catalogue, std::ostream& out, std::istream& in);