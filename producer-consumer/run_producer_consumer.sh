#!/usr/bin/env bash


echo "Building Producer-Consumer program..."
g++ -o producer_consumer producer_consumer.cpp -pthread 

echo "Running Producer-Consumer tests..."
# Arrays (or sets) of producers and consumers:
PRODUCERS=(1 4 16)
CONSUMERS=(1 2 4 16)

# You can pick a single sleep time or vary it:
SLEEP_TIME=10

# Create a results file:
RESULTS_FILE="producer_consumer_results.txt"
> "$RESULTS_FILE"  # empty it

# Outer loop: producers
for p in "${PRODUCERS[@]}"; do
  # Inner loop: consumers
  for c in "${CONSUMERS[@]}"; do
    echo "========================================" | tee -a "$RESULTS_FILE"
    echo "Test: Sleep=$SLEEP_TIME Producers=$p Consumers=$c" | tee -a "$RESULTS_FILE"
    echo "----------------------------------------" | tee -a "$RESULTS_FILE"

    # Run the program, capture its output
    ./producer_consumer "$SLEEP_TIME" "$p" "$c" | tee -a "$RESULTS_FILE"

    echo -e "\n" | tee -a "$RESULTS_FILE"
  done
done

echo "All tests complete. Results stored in $RESULTS_FILE."
