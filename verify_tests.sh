#!/bin/bash
# Proper test verification script - compares outputs with answer files
# Issue #26: Create proper test verification that compares against .ans files

REPO_DIR="/Users/wenhanlyu/.thebotcompany/dev/src/github.com/WenhanLyu/BotCompanyBenchmarkProblem004"
TEST_DATA_DIR="/Users/wenhanlyu/Documents/BotCompany/projdevbench/logs/thebotcompany/tbc-local/004/BotCompanyBenchmarkProblem004.git/data/004/1075"
CODE_EXEC="$REPO_DIR/code"
RESULTS_DIR="$REPO_DIR/test_verification_results"

mkdir -p "$RESULTS_DIR"

# Clean up previous run
rm -f "$RESULTS_DIR"/*.out "$RESULTS_DIR"/*.err "$RESULTS_DIR"/*.diff

# Color codes for terminal output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to run a single test and compare with answer
run_test() {
    local test_num=$1
    local input_file="$TEST_DATA_DIR/${test_num}.in"
    local answer_file="$TEST_DATA_DIR/${test_num}.ans"
    local output_file="$RESULTS_DIR/${test_num}.out"
    local error_file="$RESULTS_DIR/${test_num}.err"
    local diff_file="$RESULTS_DIR/${test_num}.diff"
    
    # Check if input file exists
    if [ ! -f "$input_file" ]; then
        return 2  # Test file not found
    fi
    
    # Check if answer file exists
    if [ ! -f "$answer_file" ]; then
        return 4  # Answer file not found (cannot verify)
    fi
    
    # Clean up data files before each test
    cd "$REPO_DIR"
    rm -f accounts.dat books.dat
    
    # Run with 10 second timeout using perl (macOS compatible)
    perl -e 'alarm shift; exec @ARGV' 10 "$CODE_EXEC" < "$input_file" > "$output_file" 2> "$error_file"
    local exit_code=$?
    
    # Check for timeout (SIGALRM = 142, or 124 for timeout command)
    if [ $exit_code -eq 142 ] || [ $exit_code -eq 124 ]; then
        return 3  # Timeout
    elif [ $exit_code -ne 0 ]; then
        return 1  # Failed (non-zero exit code)
    fi
    
    # Compare output with answer using diff
    diff -q "$output_file" "$answer_file" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        return 0  # PASS - output matches answer
    else
        # Save diff for analysis
        diff "$output_file" "$answer_file" > "$diff_file" 2>&1
        return 5  # FAIL - output doesn't match
    fi
}

# Parse command line arguments
if [ "$1" == "-t" ] && [ -n "$2" ]; then
    # Single test mode
    test_files=($2)
    echo "Running single test: $2"
elif [ "$1" == "-r" ] && [ -n "$2" ] && [ -n "$3" ]; then
    # Range mode: -r start end
    test_files=($(seq $2 $3))
    echo "Running tests $2 to $3"
else
    # All tests mode
    echo "Scanning for test files..."
    test_files=($(ls "$TEST_DATA_DIR"/*.in 2>/dev/null | sed 's/.*\///;s/\.in$//' | sort -n))
fi

total_tests=${#test_files[@]}

echo "Found $total_tests test files to verify"
echo "Starting verification..."
echo ""

passed=0
failed=0
timeout=0
not_found=0
no_answer=0
exit_error=0

# Arrays to track test numbers by category
passed_tests=()
failed_tests=()
timeout_tests=()
no_answer_tests=()
exit_error_tests=()

# Run all tests
for test_num in "${test_files[@]}"; do
    run_test "$test_num"
    result=$?
    
    case $result in
        0) 
            ((passed++))
            passed_tests+=($test_num)
            printf "${GREEN}✓${NC} Test %3d PASS (output matches answer)\n" $test_num
            ;;
        1) 
            ((exit_error++))
            exit_error_tests+=($test_num)
            printf "${RED}✗${NC} Test %3d FAIL (non-zero exit code)\n" $test_num
            ;;
        2) 
            ((not_found++))
            ;;
        3) 
            ((timeout++))
            timeout_tests+=($test_num)
            printf "${YELLOW}⏱${NC} Test %3d TIMEOUT (>10s)\n" $test_num
            ;;
        4)
            ((no_answer++))
            no_answer_tests+=($test_num)
            printf "${YELLOW}?${NC} Test %3d NO ANSWER FILE (cannot verify)\n" $test_num
            ;;
        5) 
            ((failed++))
            failed_tests+=($test_num)
            printf "${RED}✗${NC} Test %3d FAIL (output doesn't match answer)\n" $test_num
            ;;
    esac
done

echo ""
echo "========================================="
echo "TEST VERIFICATION RESULTS"
echo "========================================="
echo "Total Tests:       $total_tests"
echo "  ${GREEN}PASSED:${NC}          $passed (output matches answer)"
echo "  ${RED}FAILED:${NC}          $failed (output mismatch)"
echo "  ${RED}EXIT ERROR:${NC}      $exit_error (non-zero exit code)"
echo "  ${YELLOW}TIMEOUT:${NC}         $timeout (>10 seconds)"
echo "  ${YELLOW}NO ANSWER:${NC}       $no_answer (cannot verify)"
echo "========================================="
echo ""

# Calculate percentage
if [ $total_tests -gt 0 ]; then
    verifiable=$((total_tests - no_answer))
    if [ $verifiable -gt 0 ]; then
        pass_percent=$((passed * 100 / verifiable))
        echo "Pass Rate: $pass_percent% ($passed/$verifiable verifiable tests)"
    fi
fi

# Show lists of test numbers by category
if [ $passed -gt 0 ]; then
    echo ""
    echo "${GREEN}Passed Tests ($passed):${NC}"
    echo "${passed_tests[@]}" | tr ' ' '\n' | sort -n | paste -sd ' ' -
fi

if [ $failed -gt 0 ]; then
    echo ""
    echo "${RED}Failed Tests - Output Mismatch ($failed):${NC}"
    echo "${failed_tests[@]}" | tr ' ' '\n' | sort -n | paste -sd ' ' -
    echo ""
    echo "  Use: diff test_verification_results/N.out $TEST_DATA_DIR/N.ans"
    echo "  to see the differences for test N"
fi

if [ $exit_error -gt 0 ]; then
    echo ""
    echo "${RED}Failed Tests - Exit Code Error ($exit_error):${NC}"
    echo "${exit_error_tests[@]}" | tr ' ' '\n' | sort -n | paste -sd ' ' -
fi

if [ $timeout -gt 0 ]; then
    echo ""
    echo "${YELLOW}Timeout Tests ($timeout):${NC}"
    echo "${timeout_tests[@]}" | tr ' ' '\n' | sort -n | paste -sd ' ' -
fi

if [ $no_answer -gt 0 ]; then
    echo ""
    echo "${YELLOW}Tests Without Answer Files ($no_answer):${NC}"
    echo "${no_answer_tests[@]}" | tr ' ' '\n' | sort -n | paste -sd ' ' -
fi

# Save detailed summary to file
summary_file="$RESULTS_DIR/verification_summary.txt"
{
    echo "Test Verification Run: $(date)"
    echo "Test Data: $TEST_DATA_DIR"
    echo ""
    echo "Total Tests: $total_tests"
    echo "  Passed: $passed (output matches answer)"
    echo "  Failed: $failed (output mismatch)"
    echo "  Exit Error: $exit_error (non-zero exit)"
    echo "  Timeout: $timeout (>10 seconds)"
    echo "  No Answer: $no_answer (cannot verify)"
    echo ""
    echo "Verifiable Tests: $verifiable"
    if [ $verifiable -gt 0 ]; then
        echo "Pass Rate: $pass_percent%"
    fi
    echo ""
    echo "Passed Tests: ${passed_tests[@]}"
    echo ""
    echo "Failed Tests (Output Mismatch): ${failed_tests[@]}"
    echo ""
    echo "Failed Tests (Exit Error): ${exit_error_tests[@]}"
    echo ""
    echo "Timeout Tests: ${timeout_tests[@]}"
    echo ""
    echo "No Answer Tests: ${no_answer_tests[@]}"
} > "$summary_file"

echo ""
echo "Results saved to: $RESULTS_DIR/"
echo "Summary: $summary_file"
echo ""
echo "To analyze a specific failing test N, run:"
echo "  cat test_verification_results/N.out          # see actual output"
echo "  cat $TEST_DATA_DIR/N.ans     # see expected output"
echo "  diff test_verification_results/N.out $TEST_DATA_DIR/N.ans  # see differences"
echo "  cat test_verification_results/N.diff         # see saved diff"

# Exit code: 0 if all verifiable tests pass, 1 otherwise
if [ $verifiable -gt 0 ] && [ $passed -eq $verifiable ]; then
    exit 0
else
    exit 1
fi
