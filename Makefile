# Makefile for JSON Parser

CXX = g++
CXXFLAGS = -Wall -Werror -Wextra -Wpedantic -std=c++17
LDFLAGS = 

SRC = main.cpp jsonparser.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = jsonparser

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
