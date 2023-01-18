# c++ program to compile
OBJ = main.o \
	lexer.o\
	parser.o\

# compiler
CXX = g++
CXXFLAGS = -Wall -g -std=c++2a

all: $(OBJ)
	$(CXX) -o main $(OBJ)

clean:
	rm -f *.o main
