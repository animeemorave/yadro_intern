#!/bin/bash
TEST_DIR="./task_data"
clang-format -i task.cpp ledger.cpp ledger.hpp
g++ -std=c++20  task.cpp ledger.cpp -o task
for test_file in "$TEST_DIR"/*.txt; do
    base_name="${test_file%.txt}"
    sol_file="$base_name.sol"
    if [[ ! -f "$sol_file" ]]; then
        echo "Warning: Solution file for $test_file not found. Skipping."
        continue
    fi
    output=$(./task "$test_file" 2>&1)
    if diff -q <(echo "$output") "$sol_file" >/dev/null; then
        echo "Test ${base_name##*/}: PASSED"
    else
        echo "Test ${base_name##*/}: FAILED"
        echo "----------------------------------------"
        echo "Differences:"
        diff <(echo "$output") "$sol_file"
        echo "----------------------------------------"
    fi
done