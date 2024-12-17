# Makefile for the scanner project

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall -g -Wno-unused-function -Wno-sign-compare

# Source files
SRCS = main.cpp scanner.cpp parser.cpp staticSemantics.cpp codeGeneration.cpp

# Output executable
TARGET = compile

# Default target
all: $(TARGET)

# Linking the executable
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Clean target
clean:
	rm -f $(TARGET)

.PHONY: all clean

