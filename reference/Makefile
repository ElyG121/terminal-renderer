# --- Reference Build for Original Donut ---
CC       := gcc
CFLAGS   := -Wall -Wextra -O3
LDFLAGS  := -lm
TARGET   := donut

# The build happens in the current directory (flat structure)
all: $(TARGET)

$(TARGET): donut.c
	$(CC) $(CFLAGS) donut.c -o $(TARGET) $(LDFLAGS)

# To run it quickly
run: $(TARGET)
	./$(TARGET)

# Clean up only the local binary
clean:
	rm -f $(TARGET)

.PHONY: all run clean