cmake_minimum_required(VERSION 3.10)

# Project name
project(JsonParser)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -Wpedantic")

# Source files
set(SOURCES
    main.cpp
    jsonparser.cpp
)

# Create executable
add_executable(jsonparser ${SOURCES})
