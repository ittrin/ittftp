cmake_minimum_required(VERSION 3.27)
project(ittftpc)

set(CMAKE_CXX_STANDARD 17)

add_executable(ittftpc src/main.cpp
        src/tftp_packet.h)
