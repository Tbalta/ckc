CXX=g++
BUILD_DIR=../build/test
CXXFLAGS= -MMD -g -L/usr/local/lib/googletest/ -lgtest  -lgtest_main -std=c++2a -lpthread -lncurses -fexceptions -I../include

TEST_SOURCE=test.cpp
TEST_OBJ=$(addprefix $(BUILD_DIR)/, $(TEST_SOURCE:.cpp=.o))
DEPS=$(TEST_OBJ:.o=.d)

.PHONY: all
all: $(TEST_OBJ)


$(TARGET): $(TEST_OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(BUILD_DIR)/*
	$(RM) $(TARGET)

-include $(DEPS)