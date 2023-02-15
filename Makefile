# c++ program to compile
TARGET = main
BUILD_DIR = build
SOURCE= $(wildcard *.cpp)
TEST_SOURCE = $(wildcard test/*.cpp) $(filter-out main.cpp, $(SOURCE))
OBJ = $(addprefix $(BUILD_DIR)/, $(SOURCE:.cpp=.o))
TEST_OBJ= $(addprefix $(BUILD_DIR)/, $(TEST_SOURCE:.cpp=.o))
TEST_OBJ2= $(filter-out $(BUILD_DIR)/main.o, $(OBJ))

TEST_LIBS = -L/usr/local/lib/googletest/ -lgtest  -lgtest_main -lgmock -lgmock_main
DEPS = $(OBJ:.o=.d)

# compiler
CXX = g++
CXXFLAGS = -Wall -g -MMD -Iinclude `llvm-config --cxxflags --ldflags --system-libs --libs core` -std=c++2a -lpthread -lncurses -fexceptions -fsanitize=address
CXXFLAGS_CI= -Wall -g -Iinclude `llvm-config --cxxflags --ldflags --system-libs --libs core` -std=c++2a -lpthread -lncurses -fexceptions -L/usr/local/lib/googletest/ -lgtest  -lgtest_main -DTEST -Itest/include
.PHONY: directories clean compile test
test: CXXFLAGS += -DTEST -L/usr/lib/x86_64-linux-gnu/
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
	./$(TARGET) test.kc
	gcc -o demo output.o

test: $(TEST_OBJ2) $(TARGET)
	cd test && $(MAKE)
	$(CXX) -o unittest $(TEST_OBJ) $(CXXFLAGS) $(TEST_LIBS) -DTEST
	./unittest
	./test/functional/step1.sh ./$(TARGET)

.PHONY: CI
CI:
	$(CXX) -o unittest $(TEST_SOURCE) $(CXXFLAGS_CI) $(TEST_LIBS)
	$(CXX) -o $(TARGET) $(SOURCE) $(CXXFLAGS_CI)
	./unittest
	./test/functional/step1.sh ./$(TARGET)
docker:
	docker build -t ckc .docker/

-include $(DEPS)