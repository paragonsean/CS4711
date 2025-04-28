# Makefile for CPU Scheduler and Producer-Consumer Project

# — Compiler
CXX := clang++               # You can switch to g++ if needed

# — Compilation Flags
CXXFLAGS := -std=c++17 -Wall -Wextra -O3 -march=native -flto -pthread

# — Directories and Files
SCHEDULER_DIR := cpuscheduler
PRODUCER_DIR  := producer-consumer

SCHEDULER_SRC := $(SCHEDULER_DIR)/cpu_scheduler.cpp
SCHEDULER_BIN := $(SCHEDULER_DIR)/cpu_scheduler

PRODUCER_SRC  := $(PRODUCER_DIR)/producer_consumer.cpp
PRODUCER_BIN  := $(PRODUCER_DIR)/producer_consumer

# — Phony Targets
.PHONY: all clean run_scheduler run_producer

# — Default Target: Build everything
all: $(SCHEDULER_BIN) $(PRODUCER_BIN)

# — Build CPU Scheduler
$(SCHEDULER_BIN): $(SCHEDULER_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

# — Build Producer-Consumer
$(PRODUCER_BIN): $(PRODUCER_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

# — Run CPU Scheduler
run_scheduler: $(SCHEDULER_BIN)
	@echo ">>> Running CPU Scheduler..."
	cd $(SCHEDULER_DIR) && ./cpu_scheduler

# — Run Producer-Consumer
run_producer: $(PRODUCER_BIN)
	@echo ">>> Running Producer-Consumer..."
	cd $(PRODUCER_DIR) && ./producer_consumer

# — Clean Binaries
clean:
	@echo ">>> Cleaning up binaries..."
	rm -f $(SCHEDULER_BIN) $(PRODUCER_BIN)
