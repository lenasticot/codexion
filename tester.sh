#!/bin/bash

PASS=0
FAIL=0

run_test()
{
    local desc=$1
    local args=$2
    local expect=$3

    output=$(./codexion $args 2>&1)
    exit_code=$?

    if [ "$expect" = "burnout" ] && echo "$output" | grep -q "burned out"; then
        echo "PASS: $desc"
        PASS=$((PASS + 1))
    elif [ "$expect" = "success" ] && ! echo "$output" | grep -q "burned out"; then
        echo "PASS: $desc"
        PASS=$((PASS + 1))
    elif [ "$expect" = "error" ] && [ $exit_code -ne 0 ]; then
        echo "PASS: $desc"
        PASS=$((PASS + 1))
    else
        echo "FAIL: $desc"
        echo "   output: $output"
        FAIL=$((FAIL + 1))
    fi
}

# valid cases
run_test "basic fifo"      "4 1500 200 200 200 5 100 fifo"  "success"
run_test "basic edf"       "4 1500 200 200 200 5 100 edf"   "success"
run_test "single coder"    "1 800 200 200 200 5 100 fifo"   "burnout"
run_test "tight timing"    "4 500 200 200 200 5 100 fifo"   "burnout"

# invalid inputs
run_test "no args"         ""                                "error"
run_test "negative coders" "\-1 800 200 200 200 5 100 fifo" "error"
run_test "zero coders"     "0 800 200 200 200 5 100 fifo"   "error"
run_test "bad scheduler"   "4 800 200 200 200 5 100 random" "error"
run_test "non integer"     "4 abc 200 200 200 5 100 fifo"   "error"

echo ""
echo "$PASS passed, $FAIL failed"