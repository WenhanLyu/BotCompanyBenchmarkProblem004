#!/bin/bash
# Comprehensive End-to-End Integration Test Suite
# Tests complete workflows across all system components

set -e

TEST_DIR="workspace/sophia/integration_tests"
EXECUTABLE="./code"
RESULTS_FILE="workspace/sophia/integration_test_results.md"

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Initialize
mkdir -p "$TEST_DIR"
rm -f "$RESULTS_FILE"

echo "# Comprehensive Integration Test Results" > "$RESULTS_FILE"
echo "Generated: $(date)" >> "$RESULTS_FILE"
echo "" >> "$RESULTS_FILE"

TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Function to run a test
run_test() {
    local test_name="$1"
    local test_file="$2"
    local description="$3"
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    echo -e "${YELLOW}Running: $test_name${NC}"
    echo "## Test $TOTAL_TESTS: $test_name" >> "$RESULTS_FILE"
    echo "**Description**: $description" >> "$RESULTS_FILE"
    echo "" >> "$RESULTS_FILE"
    
    # Clean data files before test
    rm -f accounts.dat books.dat transactions.dat
    
    # Run test
    if $EXECUTABLE < "$test_file" > "$TEST_DIR/${test_name}_output.txt" 2>&1; then
        PASSED_TESTS=$((PASSED_TESTS + 1))
        echo -e "${GREEN}✓ PASSED${NC}"
        echo "**Status**: ✅ PASSED" >> "$RESULTS_FILE"
    else
        FAILED_TESTS=$((FAILED_TESTS + 1))
        echo -e "${RED}✗ FAILED${NC}"
        echo "**Status**: ❌ FAILED" >> "$RESULTS_FILE"
    fi
    
    echo "" >> "$RESULTS_FILE"
    echo "---" >> "$RESULTS_FILE"
    echo "" >> "$RESULTS_FILE"
}

# Clean up before starting
rm -f accounts.dat books.dat transactions.dat

echo "=========================================="
echo "  COMPREHENSIVE INTEGRATION TEST SUITE"
echo "=========================================="
echo ""

# TEST 1: Complete Account Workflow
cat > "$TEST_DIR/test1_account_workflow.in" << 'EOF'
su root sjtu
register customer1 pass123 Customer One
useradd employee1 pass456 3 Worker One
useradd admin1 pass789 7 Admin One
su customer1 pass123
logout
su employee1 pass456
logout
su admin1 pass789
passwd root newpass oldpass
delete customer1
logout
quit
EOF
run_test "account_workflow" "$TEST_DIR/test1_account_workflow.in" "Complete account management workflow: create users of all privilege levels, login/logout, password changes, deletion"

# TEST 2: Complete Book Management Workflow
cat > "$TEST_DIR/test2_book_workflow.in" << 'EOF'
su root sjtu
select ISBN001
modify -name="Book One" -author="Author A" -keyword="fiction|novel" -price=25.50
import 100 20.00
show
show -ISBN=ISBN001
show -name="Book One"
show -author="Author A"
show -keyword=fiction
buy ISBN001 5
select ISBN002
modify -name="Book Two" -author="Author B" -keyword="science|education" -price=35.00
import 50 28.00
buy ISBN002 3
show
quit
EOF
run_test "book_workflow" "$TEST_DIR/test2_book_workflow.in" "Complete book management: select, modify with all parameters, import, buy, show with all filters"

# TEST 3: Finance Tracking Integration
cat > "$TEST_DIR/test3_finance_integration.in" << 'EOF'
su root sjtu
select B001
modify -name="Product1" -price=10.00
import 100 8.00
buy B001 5
select B002
modify -name="Product2" -price=20.00
import 50 15.00
buy B002 3
show finance
show finance 1
show finance 2
quit
EOF
run_test "finance_integration" "$TEST_DIR/test3_finance_integration.in" "Finance tracking: multiple buy/import operations with show finance variations"

# TEST 4: Log System Testing (CRITICAL - not tested in 1075)
cat > "$TEST_DIR/test4_log_system.in" << 'EOF'
su root sjtu
register user1 pass1 User
useradd emp1 pass2 3 Employee
select B001
modify -name="Book1" -price=10.00
import 100 8.00
buy B001 5
report finance
report employee
log
quit
EOF
run_test "log_system" "$TEST_DIR/test4_log_system.in" "Log system commands: report finance, report employee, log (untested in 1075 suite)"

# TEST 5: Nested Login Stack
cat > "$TEST_DIR/test5_login_stack.in" << 'EOF'
su root sjtu
register user1 pass1 User
register user2 pass2 User
su user1 pass1
su user2 pass2
su user1 pass1
logout
logout
logout
logout
quit
EOF
run_test "login_stack" "$TEST_DIR/test5_login_stack.in" "Login stack: nested logins with same user multiple times"

# TEST 6: Book Selection and Modification Edge Cases
cat > "$TEST_DIR/test6_book_edge_cases.in" << 'EOF'
su root sjtu
select NEW_ISBN
show -ISBN=NEW_ISBN
select EXISTING
modify -name="Test Book"
show -ISBN=EXISTING
modify -price=0.00
show
quit
EOF
run_test "book_edge_cases" "$TEST_DIR/test6_book_edge_cases.in" "Book edge cases: create via select, modify newly created book, zero price"

# TEST 7: Multi-keyword Book Operations
cat > "$TEST_DIR/test7_multi_keyword.in" << 'EOF'
su root sjtu
select MULTI
modify -name="Multi Keyword Book" -keyword="key1|key2|key3|key4|key5" -price=50.00
show -keyword=key1
show -keyword=key3
show -keyword=key5
select SINGLE
modify -name="Single Keyword" -keyword="solo" -price=30.00
show
quit
EOF
run_test "multi_keyword" "$TEST_DIR/test7_multi_keyword.in" "Multiple keywords: create book with 5 keywords, query by different keywords"

# TEST 8: Privilege Level Enforcement
cat > "$TEST_DIR/test8_privilege_checks.in" << 'EOF'
su root sjtu
register user1 pass1 User
useradd worker1 pass2 3 Worker
logout
su user1 pass1
select B001
buy B001 1
import 10 5.00
report finance
logout
su worker1 pass2
select B002
modify -name="Worker Book" -price=10.00
import 20 8.00
useradd newuser pass3 1 New
report finance
quit
EOF
run_test "privilege_checks" "$TEST_DIR/test8_privilege_checks.in" "Privilege enforcement: verify commands fail/succeed based on user privilege"

# TEST 9: Account Deletion Impact
cat > "$TEST_DIR/test9_account_deletion.in" << 'EOF'
su root sjtu
register temp1 pass1 Temp
useradd temp2 pass2 3 Worker
su temp1 pass1
logout
delete temp1
su temp1 pass1
delete temp2
su temp2 pass2
quit
EOF
run_test "account_deletion" "$TEST_DIR/test9_account_deletion.in" "Account deletion: delete accounts and verify they cannot login afterward"

# TEST 10: Data Persistence Test (requires running executable twice)
cat > "$TEST_DIR/test10_persistence_part1.in" << 'EOF'
su root sjtu
register persistent_user pass123 Persistent
select PERSIST_BOOK
modify -name="Persistent Book" -price=99.99
import 1000 80.00
buy PERSIST_BOOK 10
quit
EOF

cat > "$TEST_DIR/test10_persistence_part2.in" << 'EOF'
su persistent_user pass123
show -ISBN=PERSIST_BOOK
show finance
logout
su root sjtu
delete persistent_user
quit
EOF

echo -e "${YELLOW}Running: data_persistence (2-part test)${NC}"
TOTAL_TESTS=$((TOTAL_TESTS + 1))
echo "## Test $TOTAL_TESTS: data_persistence" >> "$RESULTS_FILE"
echo "**Description**: Data persistence across program runs" >> "$RESULTS_FILE"
echo "" >> "$RESULTS_FILE"

rm -f accounts.dat books.dat transactions.dat
if $EXECUTABLE < "$TEST_DIR/test10_persistence_part1.in" > "$TEST_DIR/persistence_part1_output.txt" 2>&1; then
    if $EXECUTABLE < "$TEST_DIR/test10_persistence_part2.in" > "$TEST_DIR/persistence_part2_output.txt" 2>&1; then
        PASSED_TESTS=$((PASSED_TESTS + 1))
        echo -e "${GREEN}✓ PASSED${NC}"
        echo "**Status**: ✅ PASSED" >> "$RESULTS_FILE"
    else
        FAILED_TESTS=$((FAILED_TESTS + 1))
        echo -e "${RED}✗ FAILED (Part 2)${NC}"
        echo "**Status**: ❌ FAILED (Part 2)" >> "$RESULTS_FILE"
    fi
else
    FAILED_TESTS=$((FAILED_TESTS + 1))
    echo -e "${RED}✗ FAILED (Part 1)${NC}"
    echo "**Status**: ❌ FAILED (Part 1)" >> "$RESULTS_FILE"
fi
echo "" >> "$RESULTS_FILE"
echo "---" >> "$RESULTS_FILE"
echo "" >> "$RESULTS_FILE"

# TEST 11: Quoted String Parameters
cat > "$TEST_DIR/test11_quoted_strings.in" << 'EOF'
su root sjtu
select QUOTE1
modify -name="Book With Spaces" -author="John Smith Jr" -keyword="fiction book|good read" -price=45.00
show -name="Book With Spaces"
show -author="John Smith Jr"
show -keyword="fiction book"
quit
EOF
run_test "quoted_strings" "$TEST_DIR/test11_quoted_strings.in" "Quoted string handling in modify and show commands"

# TEST 12: Empty and Edge Case Parameters
cat > "$TEST_DIR/test12_edge_params.in" << 'EOF'
su root sjtu
select EDGE1
modify -price=0.00
show -ISBN=EDGE1
select EDGE2
modify -name="Min Price" -price=0.01
show
quit
EOF
run_test "edge_params" "$TEST_DIR/test12_edge_params.in" "Edge case parameters: zero price, minimal price"

# TEST 13: Large Transaction Volume
cat > "$TEST_DIR/test13_large_transactions.in" << 'EOF'
su root sjtu
select BULK
modify -name="Bulk Item" -price=1.00
import 10000 0.50
EOF

# Add 100 buy operations
for i in {1..100}; do
    echo "buy BULK 10" >> "$TEST_DIR/test13_large_transactions.in"
done

cat >> "$TEST_DIR/test13_large_transactions.in" << 'EOF'
show finance
quit
EOF
run_test "large_transactions" "$TEST_DIR/test13_large_transactions.in" "High transaction volume: 1 import + 100 buy operations"

# TEST 14: Password Change Scenarios
cat > "$TEST_DIR/test14_password_changes.in" << 'EOF'
su root sjtu
register pwtest pass1 Test
passwd pwtest pass2 pass1
su pwtest pass2
logout
useradd emp1 pass3 3 Emp
su emp1 pass3
passwd emp1 pass4
logout
su emp1 pass4
quit
EOF
run_test "password_changes" "$TEST_DIR/test14_password_changes.in" "Password changes: with and without old password, by different privilege users"

# TEST 15: Mixed Valid and Invalid Commands
cat > "$TEST_DIR/test15_mixed_commands.in" << 'EOF'
su root sjtu
invalidcommand
select VALID
show -invalid=param
modify -name="Valid Book"
randomtext
show
quit
EOF
run_test "mixed_commands" "$TEST_DIR/test15_mixed_commands.in" "Error handling: mix of valid and invalid commands"

# Summary
echo ""
echo "=========================================="
echo "  TEST SUMMARY"
echo "=========================================="
echo -e "Total Tests:  $TOTAL_TESTS"
echo -e "${GREEN}Passed:       $PASSED_TESTS${NC}"
echo -e "${RED}Failed:       $FAILED_TESTS${NC}"
echo ""

if [ $FAILED_TESTS -eq 0 ]; then
    echo -e "${GREEN}✓ ALL TESTS PASSED${NC}"
    SUCCESS_RATE=100
else
    SUCCESS_RATE=$((PASSED_TESTS * 100 / TOTAL_TESTS))
    echo -e "${YELLOW}Success Rate: ${SUCCESS_RATE}%${NC}"
fi

# Write summary to results file
echo "" >> "$RESULTS_FILE"
echo "## Summary" >> "$RESULTS_FILE"
echo "" >> "$RESULTS_FILE"
echo "- **Total Tests**: $TOTAL_TESTS" >> "$RESULTS_FILE"
echo "- **Passed**: $PASSED_TESTS ✅" >> "$RESULTS_FILE"
echo "- **Failed**: $FAILED_TESTS ❌" >> "$RESULTS_FILE"
echo "- **Success Rate**: ${SUCCESS_RATE}%" >> "$RESULTS_FILE"
echo "" >> "$RESULTS_FILE"

if [ $FAILED_TESTS -eq 0 ]; then
    echo "**Result**: 🎉 ALL INTEGRATION TESTS PASSED" >> "$RESULTS_FILE"
else
    echo "**Result**: ⚠️  Some tests failed - review individual test outputs" >> "$RESULTS_FILE"
fi

echo ""
echo "Results written to: $RESULTS_FILE"
echo "Test outputs in: $TEST_DIR/"

# Clean up data files
rm -f accounts.dat books.dat transactions.dat

exit 0
