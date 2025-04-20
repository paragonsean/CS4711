# Makefile for CPU Scheduler and Producer-Consumer Project

# Compilers
GXX = g++          # For CPU Scheduler
CLANGXX = clang++  # For Producer-Consumer

# Common flags
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Directories
SCHEDULER_DIR = cpuscheduler
PRODUCER_DIR = producer-consumer

# Sources and output binaries
SCHEDULER_SRC = $(SCHEDULER_DIR)/cpu_scheduler.cpp
SCHEDULER_BIN = $(SCHEDULER_DIR)/cpu_scheduler

PRODUCER_SRC = $(PRODUCER_DIR)/producer_consumer_shared_memory.cpp
PRODUCER_BIN = $(PRODUCER_DIR)/producer_consumer_shared_memory

# Phony targets
.PHONY: all clean run_scheduler run_producer

# Default build target
all: $(SCHEDULER_BIN) $(PRODUCER_BIN)

# Compile CPU Scheduler using g++
$(SCHEDULER_BIN): $(SCHEDULER_SRC)
	$(GXX) $(CXXFLAGS) -o $@ $^

# Compile Producer-Consumer using clang++ with pthread
$(PRODUCER_BIN): $(PRODUCER_SRC)
	$(CLANGXX) $(CXXFLAGS) -pthread -o $@ $^

# Run CPU Scheduler
run_scheduler: $(SCHEDULER_BIN)
	@echo "Running CPU Scheduler..."
	cd $(SCHEDULER_DIR) && ./cpu_scheduler

# Run Producer-Consumer
run_producer: $(PRODUCER_BIN)
	@echo "Running Producer-Consumer..."
	cd $(PRODUCER_DIR) && ./producer_consumer_shared_memory

# Clean build outputs
clean:
	@echo "Cleaning up binaries..."
	rm -f $(SCHEDULER_BIN) $(PRODUCER_BIN)
