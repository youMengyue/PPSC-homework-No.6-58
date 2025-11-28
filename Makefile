# ============================================================================
# Makefile for MPI Ring Pattern Demo
# ============================================================================
# Student: Fan Zhanhong (ID: 58)
# Course: Parallel Programming and Supercomputer Collaborative Design
# ============================================================================

# Compiler: MPI C++ compiler wrapper
# mpic++ automatically links the necessary MPI libraries
CXX = mpic++

# Compiler flags:
# -O2: Enable optimization level 2 for better performance
# -Wall: Enable all compiler warnings (optional, for debugging)
CXXFLAGS = -O2

# Target executable name
TARGET = ring_sum

# Source files
SOURCES = ring_sum.cpp

# ============================================================================
# Build Targets
# ============================================================================

# Default target: build the executable
# Usage: make or make all
all: $(TARGET)

# Rule to compile the MPI program
# $@ refers to the target (ring_sum)
# $< refers to the first dependency (ring_sum.cpp)
$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $<

# Clean target: remove compiled files
# Usage: make clean
clean:
	rm -f $(TARGET)

# ============================================================================
# Run Targets (Convenience commands)
# ============================================================================

# Run with 4 processes (default)
# Usage: make run
run: $(TARGET)
	mpirun --oversubscribe --allow-run-as-root -np 4 ./$(TARGET)

# Run with custom number of processes
# Usage: make run-np NP=8
run-np: $(TARGET)
	mpirun --oversubscribe --allow-run-as-root -np $(NP) ./$(TARGET)

# ============================================================================
# Help
# ============================================================================

# Display help information
# Usage: make help
help:
	@echo "=============================================="
	@echo "MPI Ring Pattern Demo - Build Instructions"
	@echo "=============================================="
	@echo ""
	@echo "Available targets:"
	@echo "  make          - Build the program"
	@echo "  make run      - Build and run with 4 processes"
	@echo "  make run-np NP=8  - Run with custom process count"
	@echo "  make clean    - Remove compiled files"
	@echo "  make help     - Show this help message"
	@echo ""

# Phony targets (not actual files)
.PHONY: all clean run run-np help
