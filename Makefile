# --- Terminal Renderer Makefile ---
CXX      := g++
CXXFLAGS := -Wall -Wextra -O3 -std=c++17
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

# Helper rules
run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all run clean