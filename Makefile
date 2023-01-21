# c++ program to compile
TARGET = main
OBJ = main.o \
	lexer.o\
	parser.o\
	contextProvider.o\

DEPS = $(OBJ:.o=.d)

# compiler
CXX = g++
CXXFLAGS = -Wall -g -MMD -Iinclude `llvm-config --cxxflags --ldflags --system-libs --libs core` -std=c++2a -lpthread -lncurses -fexceptions

all: $(TARGET)
$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS)

clean:
	rm -f *.o *.d main

compile:
	./$(TARGET) test.ckc 2>&1 >/dev/null | llc -o test.S
	gcc -o test test.S

-include $(DEPS)
