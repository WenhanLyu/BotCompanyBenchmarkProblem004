#!/bin/bash

# Script to analyze all test files for specific commands

DATA_DIR="./data/004/1075"
OUTPUT_DIR="./workspace/sophia"

echo "Analyzing 214 test files for command usage patterns..."
echo ""

# Initialize files
> "$OUTPUT_DIR/show_finance_tests.txt"
> "$OUTPUT_DIR/report_finance_tests.txt"
> "$OUTPUT_DIR/report_employee_tests.txt"
> "$OUTPUT_DIR/log_tests.txt"
> "$OUTPUT_DIR/command_analysis.txt"

echo "=== COMMAND ANALYSIS REPORT ===" > "$OUTPUT_DIR/command_analysis.txt"
echo "" >> "$OUTPUT_DIR/command_analysis.txt"

show_finance_count=0
report_finance_count=0
report_employee_count=0
log_count=0

# Process all .in files
for file in "$DATA_DIR"/*.in; do
    basename=$(basename "$file")
    testnum="${basename%.in}"
    
    # Check for 'show finance' command
    if grep -q "show finance" "$file"; then
        echo "$testnum" >> "$OUTPUT_DIR/show_finance_tests.txt"
        ((show_finance_count++))
    fi
    
    # Check for 'report finance' command
    if grep -q "report finance" "$file"; then
        echo "$testnum" >> "$OUTPUT_DIR/report_finance_tests.txt"
        ((report_finance_count++))
    fi
    
    # Check for 'report employee' command
    if grep -q "report employee" "$file"; then
        echo "$testnum" >> "$OUTPUT_DIR/report_employee_tests.txt"
        ((report_employee_count++))
    fi
    
    # Check for 'log' command (but not 'login' or other variants)
    if grep -qE "^log " "$file"; then
        echo "$testnum" >> "$OUTPUT_DIR/log_tests.txt"
        ((log_count++))
    fi
done

echo "SUMMARY:" >> "$OUTPUT_DIR/command_analysis.txt"
echo "--------" >> "$OUTPUT_DIR/command_analysis.txt"
echo "Total test files: 214" >> "$OUTPUT_DIR/command_analysis.txt"
echo "" >> "$OUTPUT_DIR/command_analysis.txt"
echo "Tests using 'show finance': $show_finance_count" >> "$OUTPUT_DIR/command_analysis.txt"
echo "Tests using 'report finance': $report_finance_count" >> "$OUTPUT_DIR/command_analysis.txt"
echo "Tests using 'report employee': $report_employee_count" >> "$OUTPUT_DIR/command_analysis.txt"
echo "Tests using 'log': $log_count" >> "$OUTPUT_DIR/command_analysis.txt"
echo "" >> "$OUTPUT_DIR/command_analysis.txt"

# Display results
cat "$OUTPUT_DIR/command_analysis.txt"

echo ""
echo "Detailed results saved to:"
echo "  - $OUTPUT_DIR/show_finance_tests.txt"
echo "  - $OUTPUT_DIR/report_finance_tests.txt"
echo "  - $OUTPUT_DIR/report_employee_tests.txt"
echo "  - $OUTPUT_DIR/log_tests.txt"
