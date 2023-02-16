#pragma once

#include "transport_catalogue.h"

#include <string>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <set>

void PrintBus(const std::string& str, TransportCatalogue* catalogue);

void PrintStop(const std::string& str, TransportCatalogue* catalogue);

void OutputInfo(const std::string& output, TransportCatalogue* catalogue);