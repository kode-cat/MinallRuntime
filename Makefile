CC = gcc
CFLAGS = -O3 -Wall -Wextra -std=c99 -ffast-math -march=native -funroll-loops -fomit-frame-pointer -finline-functions
TARGET = minall
SOURCES = main.c lexer.c parser.c interpreter.c memory.c benchmark.c fastloop.c

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(SOURCES) minall.h
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

# Debug build
debug: CFLAGS = -g -Wall -Wextra -std=c99 -DDEBUG
debug: $(TARGET)

# Performance build with maximum optimizations
performance: CFLAGS = -O3 -Wall -Wextra -std=c99 -ffast-math -march=native -flto -DNDEBUG
performance: $(TARGET)

# Run tests
test: $(TARGET)
	./$(TARGET) test.js

# Run benchmarks
benchmark: $(TARGET)
	./$(TARGET) benchmark.js --benchmark

# Clean build artifacts
clean:
	rm -f $(TARGET)

# Install (copy to /usr/local/bin)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# Uninstall
uninstall:
	rm -f /usr/local/bin/$(TARGET)

# Help
help:
	@echo "MinAll JavaScript Runtime Build System"
	@echo "Available targets:"
	@echo "  all         - Build the runtime (default)"
	@echo "  debug       - Build with debug symbols"
	@echo "  performance - Build with maximum optimizations"
	@echo "  test        - Run test script"
	@echo "  benchmark   - Run performance benchmarks"
	@echo "  clean       - Remove build artifacts"
	@echo "  install     - Install to /usr/local/bin"
	@echo "  uninstall   - Remove from /usr/local/bin"
	@echo "  help        - Show this help"

.PHONY: all debug performance test benchmark clean install uninstall help