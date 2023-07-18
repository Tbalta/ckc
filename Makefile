# c++ program to compile
TARGET = gkc
TEST_EXEC = unittest
BUILD_DIR = build
SOURCE= $(wildcard src/*.cpp)
TEST_SOURCE = $(wildcard test/*.cpp) $(filter-out src/main.cpp, $(SOURCE))
OBJ = $(addprefix $(BUILD_DIR)/, $(SOURCE:.cpp=.o))
TEST_OBJ= $(addprefix $(BUILD_DIR)/, $(TEST_SOURCE:.cpp=.o))
TEST_OBJ2= $(filter-out $(BUILD_DIR)/src/main.o, $(OBJ))

TEST_LIBS = -L/usr/local/lib/googletest/ -lgtest  -lgtest_main -lgmock -lgmock_main
DEPS = $(OBJ:.o=.d)

# compiler
CXX = g++
CXXFLAGS = -Wall -g -MMD -Iinclude `llvm-config --cxxflags --ldflags --system-libs --libs core` -std=c++2a -lpthread -lncurses -fexceptions
CXXFLAGS_CI= -Wall -g -Iinclude `llvm-config --cxxflags --ldflags --system-libs --libs core` -std=c++2a -lpthread -lncurses -fexceptions -L/usr/local/lib/googletest/ -lgtest  -lgtest_main -DTEST -DPROD -Itest/include -fsanitize=address
.PHONY: directories clean compile test
all: directories $(TARGET)
test: CXXFLAGS += -DTEST -DPROD -L/usr/lib/x86_64-linux-gnu/
prod: CXXFLAGS += -DPROD
prod: $(TARGET)
directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/test
	mkdir -p $(BUILD_DIR)/src


$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS)

$(BUILD_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) -r $(BUILD_DIR)/test
	$(RM) -r $(BUILD_DIR)/*
	$(RM) $(TARGET)
	$(RM) $(TEST_EXEC)
	$(RM) *.o
	$(RM) *.out
	cd test && $(MAKE) clean

compile: $(TARGET)
	./$(TARGET) test.kc
	gcc -o demo output.o

test: directories $(TEST_OBJ2) $(TARGET)
	cd test && $(MAKE)
	$(CXX) -o unittest $(TEST_OBJ) $(CXXFLAGS) $(TEST_LIBS) -DTEST
	./unittest
	./test/functional/step1.sh ./$(TARGET)
	./test/functional/step2.sh ./$(TARGET)

.PHONY: CI
CI:
	$(CXX) -o $(TEST_EXEC) $(TEST_SOURCE) $(CXXFLAGS_CI) $(TEST_LIBS)
	$(CXX) -o $(TARGET) $(SOURCE) $(CXXFLAGS_CI)
	./$(TEST_EXEC)
	./test/functional/step1.sh ./$(TARGET)
	./test/functional/step2.sh ./$(TARGET)



-include $(DEPS)