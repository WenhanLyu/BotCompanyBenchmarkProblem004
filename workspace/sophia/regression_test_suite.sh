#!/bin/bash
# Regression Test Suite - Run against existing test files
# Verify system works with all previously created test files

set -e

EXECUTABLE="./code"
RESULTS_FILE="workspace/sophia/regression_test_results.md"
TEST_COUNT=0
PASS_COUNT=0
FAIL_COUNT=0

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo "# Regression Test Results" > "$RESULTS_FILE"
echo "Generated: $(date)" >> "$RESULTS_FILE"
echo "" >> "$RESULTS_FILE"

echo "=========================================="
echo "  REGRESSION TEST SUITE"
echo "=========================================="
echo ""

# Function to run a test file
run_regression_test() {
    local test_file="$1"
    local test_name=$(basename "$test_file" .in)
    
    TEST_COUNT=$((TEST_COUNT + 1))
    
    echo -e "${YELLOW}Testing: $test_name${NC}"
    
    # Clean data files
    rm -f accounts.dat books.dat transactions.dat
    
    # Run test
    if $EXECUTABLE < "$test_file" > "workspace/sophia/${test_name}_output.txt" 2>&1; then
        PASS_COUNT=$((PASS_COUNT + 1))
        echo -e "${GREEN}✓ PASSED${NC}"
        echo "- ✅ $test_name" >> "$RESULTS_FILE"
    else
        FAIL_COUNT=$((FAIL_COUNT + 1))
        echo -e "${RED}✗ FAILED${NC}"
        echo "- ❌ $test_name (exit code: $?)" >> "$RESULTS_FILE"
    fi
}

# Test all demo files
echo "Testing demo files..."
for test_file in demo*.in; do
    if [ -f "$test_file" ]; then
        run_regression_test "$test_file"
    fi
done

# Test regression test files
echo ""
echo "Testing regression test files..."
for test_file in regr_test*.in; do
    if [ -f "$test_file" ]; then
        run_regression_test "$test_file"
    fi
done

# Test quoted string tests
echo ""
echo "Testing quoted string test files..."
for test_file in test_quoted*.in; do
    if [ -f "$test_file" ]; then
        run_regression_test "$test_file"
    fi
done

# Test other test files
echo ""
echo "Testing additional test files..."
for test_file in test*.in step*.in; do
    if [ -f "$test_file" ]; then
        # Skip already tested files
        if [[ ! "$test_file" =~ test_quoted ]]; then
            run_regression_test "$test_file"
        fi
    fi
done

# Summary
echo ""
echo "=========================================="
echo "  REGRESSION TEST SUMMARY"
echo "=========================================="
echo ""
echo "Total Tests:  $TEST_COUNT"
echo -e "${GREEN}Passed:       $PASS_COUNT${NC}"
echo -e "${RED}Failed:       $FAIL_COUNT${NC}"
echo ""

SUCCESS_RATE=$((PASS_COUNT * 100 / TEST_COUNT))
echo "Success Rate: ${SUCCESS_RATE}%"

# Write summary
echo "" >> "$RESULTS_FILE"
echo "## Summary" >> "$RESULTS_FILE"
echo "- **Total**: $TEST_COUNT" >> "$RESULTS_FILE"
echo "- **Passed**: $PASS_COUNT ✅" >> "$RESULTS_FILE"
echo "- **Failed**: $FAIL_COUNT ❌" >> "$RESULTS_FILE"
echo "- **Success Rate**: ${SUCCESS_RATE}%" >> "$RESULTS_FILE"

if [ $FAIL_COUNT -eq 0 ]; then
    echo -e "\n${GREEN}✓ ALL REGRESSION TESTS PASSED${NC}"
    echo "" >> "$RESULTS_FILE"
    echo "**Result**: 🎉 ALL REGRESSION TESTS PASSED" >> "$RESULTS_FILE"
else
    echo -e "\n${YELLOW}⚠️  Some tests failed${NC}"
    echo "" >> "$RESULTS_FILE"
    echo "**Result**: ⚠️  $FAIL_COUNT test(s) failed" >> "$RESULTS_FILE"
fi

echo ""
echo "Results written to: $RESULTS_FILE"

# Clean up
rm -f accounts.dat books.dat transactions.dat

exit 0
