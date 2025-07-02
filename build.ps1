# build.ps1 for JSON Parser

# Compile the project
g++ -Wall -Werror -Wextra -Wpedantic -std=c++17 -o jsonparser main.cpp jsonparser.cpp

Write-Host "Build completed successfully."
