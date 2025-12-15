# Makefile for Git Contribution Generator

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11
DEBUG_FLAGS = -g -O0 -DDEBUG -fsanitize=address -fsanitize=undefined
PREFIX = /usr/local

# Target executable
TARGET = git-contrib
SOURCE = main.c

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)
	@echo "Build complete: $(TARGET)"

# Debug build
debug: CFLAGS = $(DEBUG_FLAGS) -Wall -Wextra -std=c11
debug: clean $(TARGET)
	@echo "Debug build complete"

# Install to system
install: $(TARGET)
	@echo "Installing $(TARGET) to $(PREFIX)/bin"
	install -m 755 $(TARGET) $(PREFIX)/bin/
	@echo "Installation complete"

# Uninstall from system
uninstall:
	@echo "Removing $(TARGET) from $(PREFIX)/bin"
	rm -f $(PREFIX)/bin/$(TARGET)
	@echo "Uninstall complete"

# Clean build artifacts
clean:
	rm -f $(TARGET)
	rm -rf repository-*
	@echo "Clean complete"

# Run with default options
run: $(TARGET)
	./$(TARGET)

# Run with example options
example: $(TARGET)
	./$(TARGET) --no-weekends --frequency 75 --max-commits 8

# Display help
help:
	@echo "Git Contribution Generator - Makefile targets:"
	@echo ""
	@echo "  make             - Build the project (default)"
	@echo "  make debug       - Build with debug symbols"
	@echo "  make install     - Install to $(PREFIX)/bin (requires sudo)"
	@echo "  make uninstall   - Remove from $(PREFIX)/bin (requires sudo)"
	@echo "  make clean       - Remove build artifacts and test repositories"
	@echo "  make run         - Build and run with default options"
	@echo "  make example     - Build and run with example options"
	@echo "  make test        - Run basic tests"
	@echo "  make help        - Display this help message"
	@echo ""

# Basic test
test: $(TARGET)
	@echo "Running basic test..."
	./$(TARGET) --days-before 7 --days-after 0 --frequency 50
	@echo "Test complete - check the generated repository directory"

.PHONY: all debug install uninstall clean run example help test
