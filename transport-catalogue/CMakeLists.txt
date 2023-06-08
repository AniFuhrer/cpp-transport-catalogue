cmake_minimum_required(VERSION 3.25)
project(BusGuide2)

set(CMAKE_CXX_STANDARD 17)

add_executable(BusGuide2 main.cpp domain.cpp domain.h geo.cpp geo.h json.cpp json.h json_reader.cpp json_reader.h map_renderer.cpp map_renderer.h request_handler.cpp request_handler.h svg.cpp svg.h transport_catalogue.cpp transport_catalogue.h)
