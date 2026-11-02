# --- Terminal Renderer Makefile ---
CXX      := g++
CXXFLAGS := -Wall -Wextra -O3 -std=c++17 -MMD -MP
LDFLAGS  := -lm

# Directories
SRC_DIR  := src
INC_DIR  := include
OBJ_DIR  := obj
BIN_DIR  := bin

# Files
TARGET   := $(BIN_DIR)/renderer
SRCS     := $(wildcard $(SRC_DIR)/*.cpp)
OBJS     := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
SRC_OBJS := $(filter-out $(OBJ_DIR)/main.o,$(OBJS))

TEST_DIR    := tests
TEST_SRCS   := $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJS   := $(patsubst $(TEST_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(TEST_SRCS))
TEST_TARGET := $(BIN_DIR)/test_runner

# Default rule
all: $(TARGET)

# Link the final binary
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

# Create directories if they don't exist
$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

# Test build rules
$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

$(TEST_TARGET): $(SRC_OBJS) $(TEST_OBJS) | $(BIN_DIR)
	$(CXX) $(SRC_OBJS) $(TEST_OBJS) -o $(TEST_TARGET) $(LDFLAGS)

# Helper rules
run: $(TARGET)
	./$(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all run test clean

-include $(OBJS:.o=.d)