# c++ program to compile
TARGET = main
BUILD_DIR = build
SOURCE= \
	main.o\
	lexer.o\
	parser.o\
	contextProvider.o 
TEST_SOURCE = $(wildcard test/*.cpp)
OBJ = $(addprefix $(BUILD_DIR)/, $(SOURCE:.cpp=.o))
TEST_OBJ= $(addprefix $(BUILD_DIR)/, $(TEST_SOURCE:.cpp=.o)) $(filter-out $(BUILD_DIR)/main.o, $(OBJ))

TEST_LIBS = -L/usr/local/lib/googletest/ -lgtest  -lgtest_main
DEPS = $(OBJ:.o=.d)

# compiler
CXX = g++
CXXFLAGS = -Wall -g -MMD -Iinclude `llvm-config --cxxflags --ldflags --system-libs --libs core` -std=c++2a -lpthread -lncurses -fexceptions

.PHONY: directories clean compile test

all: directories $(TARGET)
directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/test


$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS)

$(BUILD_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -r $(BUILD_DIR)/test
	rm -f $(BUILD_DIR)/*
	$(RM) $(TARGET)
	cd test && $(MAKE) clean

compile: $(TARGET)
	./$(TARGET) test.ckc 2>&1 >/dev/null | llc -o test.S
	gcc -o demo test.S

test: $(OBJ)
	cd test && $(MAKE)
	$(CXX) -o unittest $(TEST_OBJ) $(CXXFLAGS) $(TEST_LIBS)
	./unittest

-include $(DEPS)