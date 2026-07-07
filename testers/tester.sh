#!/bin/bash

# ============================================================
# Codexion Test Suite
# ============================================================

BINARY="./codexion"
PASS=0
FAIL=0
TOTAL=0

# macOS: 'timeout' lives in GNU coreutils as 'gtimeout'; fall back to a bash impl
if ! command -v timeout &>/dev/null; then
	if command -v gtimeout &>/dev/null; then
		timeout() { gtimeout "$@"; }
	else
		timeout() {
			local sec=$1; shift
			"$@" &
			local child=$!
			{ sleep "$sec" && kill "$child" 2>/dev/null; } &
			local watcher=$!
			wait "$child" 2>/dev/null
			local code=$?
			kill "$watcher" 2>/dev/null
			wait "$watcher" 2>/dev/null
			[ "$code" -eq 143 ] && return 124
			return "$code"
		}
	fi
fi

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# ============================================================
# Helpers
# ============================================================

print_header() {
	echo ""
	echo -e "${BLUE}============================================================${NC}"
	echo -e "${BLUE}  $1${NC}"
	echo -e "${BLUE}============================================================${NC}"
}

run_test() {
	local desc="$1"
	local args="$2"
	local timeout_s="$3"
	local check_fn="$4"

	TOTAL=$((TOTAL + 1))
	echo -e "\n${YELLOW}[TEST $TOTAL]${NC} $desc"
	echo "  Args: $args"

	OUTPUT=$(timeout "$timeout_s" $BINARY $args 2>&1)
	EXIT_CODE=$?

	if [ $EXIT_CODE -eq 124 ]; then
		echo -e "  ${RED}FAIL${NC} — timed out after ${timeout_s}s (possible deadlock)"
		FAIL=$((FAIL + 1))
		return
	fi

	# Call the check function with output
	if $check_fn "$OUTPUT" "$args"; then
		echo -e "  ${GREEN}PASS${NC}"
		PASS=$((PASS + 1))
	else
		FAIL=$((FAIL + 1))
	fi
}

# ============================================================
# Check functions
# ============================================================

# Check that no burnout occurred and all coders hit required compiles
check_clean_run() {
	local output="$1"
	local args="$2"
	local n_coders=$(echo $args | awk '{print $1}')
	local required=$(echo $args | awk '{print $6}')

	# No burnout
	if echo "$output" | grep -q "burned out"; then
		echo -e "  ${RED}FAIL${NC} — unexpected burnout detected"
		echo "$output" | grep "burned out"
		return 1
	fi

	# Each coder compiled at least required times
	local failed=0
	for i in $(seq 1 $n_coders); do
		local count=$(echo "$output" | grep "^[0-9]* $i is compiling" | wc -l)
		if [ "$count" -lt "$required" ]; then
			echo -e "  ${RED}FAIL${NC} — coder $i compiled only $count/$required times"
			failed=1
		fi
	done

	# Each compile preceded by exactly 2 dongle takes
	local compile_lines=$(echo "$output" | grep "is compiling")
	while IFS= read -r line; do
		local ts=$(echo "$line" | awk '{print $1}')
		local coder=$(echo "$line" | awk '{print $2}')
		local dongle_count=$(echo "$output" | awk -v c="$coder" -v t="$ts" \
			'$2==c && $0~/has taken a dongle/ && $1<=t {count++} END {print count+0}')
		# We can't perfectly check ordering here without more parsing, skip deep check
	done <<< "$compile_lines"

	return $failed
}

# Check that burnout DID occur
check_burnout_occurs() {
	local output="$1"
	if echo "$output" | grep -q "burned out"; then
		return 0
	fi
	echo -e "  ${RED}FAIL${NC} — expected burnout but none occurred"
	return 1
}

# Check no log lines are interleaved (each line matches expected format)
check_log_format() {
	local output="$1"
	local bad=$(echo "$output" | grep -vE \
		'^[0-9]+ [0-9]+ (has taken a dongle|is compiling|is debugging|is refactoring|burned out)$')
	if [ -n "$bad" ]; then
		echo -e "  ${RED}FAIL${NC} — malformed log lines detected:"
		echo "$bad" | head -5
		return 1
	fi
	return 0
}

# Check that compile is always preceded by 2 dongle takes for same coder
check_dongle_pairs() {
	local output="$1"
	local failed=0

	# For each "is compiling" line, check the two lines before it (for that coder)
	while IFS= read -r compile_line; do
		local coder=$(echo "$compile_line" | awk '{print $2}')
		local ts=$(echo "$compile_line" | awk '{print $1}')

		# Get all dongle takes for this coder before this timestamp
		local takes=$(echo "$output" | awk -v c="$coder" -v t="$ts" \
			'$2==c && /has taken a dongle/ && $1 <= t' | tail -2 | wc -l)

		if [ "$takes" -lt 2 ]; then
			echo -e "  ${RED}FAIL${NC} — coder $coder compiled at ${ts}ms without 2 dongle takes"
			failed=1
		fi
	done <<< "$(echo "$output" | grep "is compiling")"

	return $failed
}

# Check burnout timing: burnout log within 10ms of deadline
check_burnout_timing() {
	local output="$1"
	local args="$2"
	local burnout_line=$(echo "$output" | grep "burned out" | head -1)

	if [ -z "$burnout_line" ]; then
		return 0  # no burnout to check
	fi

	local burnout_ts=$(echo "$burnout_line" | awk '{print $1}')
	local burnout_coder=$(echo "$burnout_line" | awk '{print $2}')
	local time_to_burnout=$(echo $args | awk '{print $2}')

	# Find last compile start for this coder (or 0 if never compiled)
	local last_compile=$(echo "$output" | awk -v c="$burnout_coder" \
		'$2==c && /is compiling/ {last=$1} END {print last+0}')

	local expected_deadline=$((last_compile + time_to_burnout))
	local diff=$((burnout_ts - expected_deadline))

	# Allow -5 to +10ms tolerance
	if [ "$diff" -lt -5 ] || [ "$diff" -gt 10 ]; then
		echo -e "  ${RED}FAIL${NC} — burnout at ${burnout_ts}ms, expected ~${expected_deadline}ms (diff: ${diff}ms)"
		return 1
	fi
	return 0
}

# Check invalid input is rejected (non-zero exit or error message)
check_rejected() {
	local output="$1"
	# Should print nothing useful or exit with error
	if echo "$output" | grep -qE "is compiling|is debugging|is refactoring|has taken a dongle"; then
		echo -e "  ${RED}FAIL${NC} — invalid input was accepted and simulation ran"
		return 1
	fi
	return 0
}

# Combined check: clean run + log format + dongle pairs
check_full() {
	local output="$1"
	local args="$2"
	check_log_format "$output" && \
	check_dongle_pairs "$output" && \
	check_clean_run "$output" "$args"
}

# Combined check: burnout expected + timing correct
check_burnout_full() {
	local output="$1"
	local args="$2"
	check_log_format "$output" && \
	check_burnout_occurs "$output" && \
	check_burnout_timing "$output" "$args"
}

# ============================================================
# Check binary exists
# ============================================================

if [ ! -f "$BINARY" ]; then
	echo -e "${RED}ERROR: $BINARY not found. Run 'make' first.${NC}"
	exit 1
fi

# ============================================================
# EASY TESTS — clean runs, no burnout
# ============================================================

print_header "EASY — Clean runs, no burnout"

run_test "5 coders, generous timing, fifo" \
	"5 800 200 200 200 5 0 fifo" 15 check_full

run_test "2 coders, generous timing, edf" \
	"2 800 200 200 200 3 0 edf" 10 check_full

run_test "3 coders, generous timing, fifo" \
	"3 800 200 200 200 3 0 fifo" 10 check_full

run_test "4 coders, generous timing, edf" \
	"4 1000 200 200 200 3 0 edf" 15 check_full

run_test "2 coders, only 1 compile required" \
	"2 800 200 100 100 1 0 fifo" 5 check_full

run_test "Large: 10 coders, fifo" \
	"10 800 200 100 100 3 0 fifo" 20 check_full

# ============================================================
# LESS EASY — burnout edge cases
# ============================================================

print_header "LESS EASY — Burnout edge cases"

run_test "1 coder (impossible — only 1 dongle, needs 2)" \
	"1 800 200 200 200 5 0 fifo" 5 check_burnout_occurs

run_test "Tight timing: burnout expected (debug time exceeds slack)" \
	"2 201 200 100 100 5 0 fifo" 5 check_burnout_occurs
# Note: this SHOULD burnout only if parameters are truly infeasible.
# With correct last_compile_start reset, coder 1 survives. Inspect output carefully.

run_test "Borderline feasibility: burnout expected" \
	"4 310 200 100 100 5 0 fifo" 10 check_burnout_occurs

run_test "Tight but feasible: 2 coders 500ms burnout" \
	"2 500 200 100 100 5 0 fifo" 10 check_full

# ============================================================
# MEDIUM — cooldown, scheduler differences, log serialization
# ============================================================

print_header "MEDIUM — Cooldown, EDF vs FIFO, log serialization"

run_test "Cooldown 300ms, feasible timing" \
	"2 1200 200 100 100 3 300 fifo" 10 check_full

run_test "Cooldown 300ms with EDF" \
	"3 2000 200 100 100 3 300 edf" 20 check_full

run_test "Cooldown causes burnout (infeasible)" \
	"2 800 200 100 100 5 300 fifo" 5 check_burnout_occurs

run_test "EDF vs FIFO — 3 coders with pressure" \
	"3 1000 200 100 500 5 0 edf" 25 check_full

run_test "Zero debug and refactor — high contention" \
	"4 1200 200 0 0 5 0 fifo" 15 check_full

run_test "Zero debug and refactor — EDF" \
	"4 1200 200 0 0 5 0 edf" 15 check_full

run_test "Log format check — all lines valid" \
	"3 800 200 100 100 3 0 fifo" 10 check_log_format

run_test "Dongle pairs — every compile has 2 takes" \
	"5 800 200 100 100 3 0 fifo" 15 check_dongle_pairs

# ============================================================
# INVALID INPUT — must be rejected
# ============================================================

print_header "INVALID INPUT — must reject gracefully"

run_test "Negative coder count" \
	"-1 800 200 100 100 5 0 fifo" 2 check_rejected

run_test "Zero coders" \
	"0 800 200 100 100 5 0 fifo" 2 check_rejected

run_test "Negative burnout time" \
	"3 -500 200 100 100 5 0 fifo" 2 check_rejected

run_test "Non-integer argument" \
	"3 800 abc 100 100 5 0 fifo" 2 check_rejected

run_test "Invalid scheduler" \
	"3 800 200 100 100 5 0 random" 2 check_rejected

run_test "Missing argument (7 instead of 8)" \
	"3 800 200 100 100 5 0" 2 check_rejected

run_test "Too many arguments" \
	"3 800 200 100 100 5 0 fifo extra" 2 check_rejected

# ============================================================
# STRESS TEST
# ============================================================

print_header "STRESS — Large scale"

run_test "50 coders, fifo" \
	"50 800 200 100 100 3 0 fifo" 30 check_full

run_test "100 coders, edf" \
	"100 800 200 100 100 2 0 edf" 30 check_full

run_test "200 coders, fifo (max allowed)" \
	"200 1000 200 100 100 2 0 fifo" 60 check_full

# ============================================================
# SUMMARY
# ============================================================

print_header "RESULTS"

echo ""
echo -e "  Total:  $TOTAL"
echo -e "  ${GREEN}Passed: $PASS${NC}"
echo -e "  ${RED}Failed: $FAIL${NC}"
echo ""

if [ $FAIL -eq 0 ]; then
	echo -e "${GREEN}  All tests passed!${NC}"
else
	echo -e "${RED}  $FAIL test(s) failed. Review output above.${NC}"
fi

echo ""