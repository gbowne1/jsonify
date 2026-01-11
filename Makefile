LINTER_TEST_SRC = linter_test.cpp jsonparser.cpp jsonlinter.cpp jsonformatter.cpp
LINTER_TEST_OBJ = $(LINTER_TEST_SRC:.cpp=.o)
LINTER_TEST_TARGET = linter_test

$(LINTER_TEST_TARGET): $(LINTER_TEST_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $(LINTER_TEST_SRC)

test: $(LINTER_TEST_TARGET)
	./$(LINTER_TEST_TARGET)
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
