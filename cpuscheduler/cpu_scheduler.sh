#!/usr/bin/env bash


echo "Building CPU Scheduling program..."
g++ -o cpu_scheduler cpu_scheduler.cpp

echo "Running CPU Scheduling program..."
# Our code expects datafile1.txt in the same directory.
./cpu_scheduler > cpu_scheduler_results.txt

echo "Done! Results are in cpu_scheduler_results.txt"
