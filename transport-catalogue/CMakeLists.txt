cmake_minimum_required(VERSION 3.24)
project(BusGuide)

set(CMAKE_CXX_STANDARD 17)

add_executable(BusGuide main.cpp input_reader.cpp stat_reader.cpp transport_catalogue.cpp)
