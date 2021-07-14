# cmake_minimum_required(VERSION <specify CMake version here>)
project(DOEGantryV2)

set(CMAKE_CXX_STANDARD 14)

add_executable(DOEGantryV2 main.cpp Drill.cpp Drill.h Gantry.cpp Gantry.h Electromagnets.cpp Electromagnets.h Arduino.cpp Arduino.h Fans.cpp Fans.h)