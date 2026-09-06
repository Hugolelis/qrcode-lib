CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -g -Iincludes

TARGET := build/qrcode
TEST_TARGET := build/tests

LIB_SOURCES := $(wildcard src/*.cpp)
APP_SOURCES := bin/main.cpp
TEST_SOURCES := $(LIB_SOURCES) tests/test_runner.cpp

SOURCE := $(LIB_SOURCES) $(APP_SOURCES)

.PHONY: all run clean test

all: $(TARGET)

$(TARGET): $(SOURCE)
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(SOURCE) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_SOURCES)
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(TEST_SOURCES) -o $(TEST_TARGET)

clean:
	rm -rf build