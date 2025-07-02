@echo off
REM build.bat for JSON Parser

REM Compile the project
g++ -Wall -Werror -Wextra -Wpedantic -std=c++17 -o jsonparser main.cpp jsonparser.cpp

echo Build completed successfully.
