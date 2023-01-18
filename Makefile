# c++ program to compile
TARGET = main
OBJ = main.o \
	lexer.o\
	parser.o\

DEPS = $(OBJ:.o=.d)

# compiler
CXX = g++
CXXFLAGS = -Wall -g -std=c++2a -MD -Iinclude 

all: $(TARGET)
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f *.o main

-include $(DEPS)
