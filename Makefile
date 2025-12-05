CXX = g++
CXXFLAGS = -Wall -Werror -Wextra -Wpedantic -pedantic -pedantic-errors -std=c++17
LDFLAGS = 

SRC = main.cpp jsonparser.cpp jsonlinter.cpp jsonformatter.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = jsonify

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
