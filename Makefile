# Compiler and flags
CXX = g++
CXXFLAGS = -Iinclude -std=c++17 -O2 -MMD -MP -g -Wall -Werror

# Directories
SRC_DIR = src
TEST_DIR = test
BUILD_DIR = obj
BIN_DIR = bin
DATA_DIR = data

# Source files
SOURCES = $(SRC_DIR)/ConsoleApp.cpp $(SRC_DIR)/DataLoader.cpp
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEPS = $(OBJECTS:.o=.d)

# Exclude ConsoleApp.o from test dependencies
NON_APP_OBJECTS = $(filter-out $(BUILD_DIR)/ConsoleApp.o, $(OBJECTS))

# Test files
TEST_SOURCES = $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJECTS = $(TEST_SOURCES:$(TEST_DIR)/%.cpp=$(BUILD_DIR)/%.o)
TEST_DEPS = $(TEST_OBJECTS:.o=.d)
TEST_TARGET = $(BIN_DIR)/tests

# Executable target
TARGET = $(BIN_DIR)/ConsoleApp

# Dataset directory
DATASET = siftsmall

# Arguments for the executable
K = 50
L = 100
R = 60
A = 2
BASE_DATASET = $(DATA_DIR)/$(DATASET)/$(DATASET)_base.fvecs
QUERY_DATASET = $(DATA_DIR)/$(DATASET)/$(DATASET)_query.fvecs
GROUND_TRUTH = $(DATA_DIR)/$(DATASET)/$(DATASET)_groundtruth.ivecs

# Build rules
all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile test files with non-app objects
$(TEST_TARGET): $(TEST_OBJECTS) $(NON_APP_OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lgtest -lgtest_main -pthread

# Compile source files and generate .d files for dependencies
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# Compile test files and generate .d files for dependencies
$(BUILD_DIR)/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Run the program with arguments
run: $(TARGET)
	./$(TARGET) $(K) $(L) $(R) $(A) $(BASE_DATASET) $(QUERY_DATASET) $(GROUND_TRUTH)

# Run all tests, or a specific test if TEST_FILTER is provided
test: $(TEST_TARGET)
	@if [ -n "$(TEST_FILTER)" ]; then \
		./$(TEST_TARGET) --gtest_filter=$(TEST_FILTER); \
	else \
		./$(TEST_TARGET); \
	fi

# Debug with Valgrind and run with arguments
debug: CXXFLAGS += -O0 -g
debug: clean $(TARGET)
	valgrind --leak-check=full ./$(TARGET) $(K) $(L) $(R) $(A) $(BASE_DATASET) $(QUERY_DATASET) $(GROUND_TRUTH)

# Include the dependency files if they exist
-include $(DEPS) $(TEST_DEPS)

.PHONY: all clean run debug test
