CXX=g++
BUILD_DIR=../build/test
CXXFLAGS= -MMD -g -L/usr/local/lib/googletest/ -lgtest  -lgtest_main -std=c++2a -lpthread -lncurses -fexceptions -I../include -Iinclude -L/usr/lib/x86_64-linux-gnu/ -lgmock -lgmock_main

TEST_SOURCE= $(wildcard *.cpp)
TEST_OBJ=$(addprefix $(BUILD_DIR)/, $(TEST_SOURCE:.cpp=.o))
DEPS=$(TEST_OBJ:.o=.d)

.PHONY: all
all: $(TEST_OBJ)


$(TARGET): $(TEST_OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS) -DTEST

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@ -DTEST

clean:
	$(RM) $(BUILD_DIR)/*
	$(RM) $(TARGET)

-include $(DEPS)