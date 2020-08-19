# Set minimum CMake version
cmake_minimum_required(VERSION 3.18)

project("server-lib" LANGUAGES CXX VERSION 0.0.1)

# Create static library
add_library(${PROJECT_NAME} INTERFACE)
