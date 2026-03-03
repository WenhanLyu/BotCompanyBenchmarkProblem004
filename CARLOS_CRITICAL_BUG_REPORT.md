# CRITICAL BUG REPORT - Carlos

**Date**: March 2, 2026  
**Reporter**: Carlos (Test Execution Verifier)  
**Severity**: CRITICAL - P0  
**Status**: Blocks ACMOJ submission  

---

## Summary

Comprehensive regression testing revealed a **CRITICAL BUG** that breaks all modify/show commands using quoted strings.

**Impact**: System cannot handle book names, authors, or keywords with spaces.

---

## The Bug

### Commands Affected:
- `modify -name="..."`  ❌ BROKEN
- `modify -author="..."`  ❌ BROKEN
- `modify -keyword="..."`  ❌ BROKEN
- `show -name="..."`  ❌ BROKEN
- `show -author="..."`  ❌ BROKEN
- `show -keyword="..."`  ❌ BROKEN

### Example:
```bash
su root sjtu
select BOOK001
modify -name="Test Book"
show -ISBN=BOOK001
```

**Expected**: Book name set to "Test Book"  
**Actual**: Returns "Invalid", book name remains empty

---

## Root Cause

**File**: `src/main.cpp`, line 305  
**Issue**: Stringstream tokenization breaks on spaces inside quotes

```cpp
std::string param;
while (ss >> param) {
    // This reads only "-name="Test" - stops at space!
    // Rest of string "Book"" is lost
}
```

When parsing `-name="Test Book"`:
1. `ss >> param` reads `-name="Test` (stops at space)
2. Closing quote and "Book"" are never read
3. Parse fails → Invalid

---

## Testing Results

**Test Suite**: 7 regression tests  
**Pass Rate**: 42.9% (3/7)  
**Critical Bugs**: 1  
**Minor Issues**: 1

### Failed Tests:
1. `test_double_quote_modify` - Can't modify book name
2. `test_book_operations` - Full book workflow broken
3. `test_log_privilege` - Extra newline (minor)

### Passed Tests:
1. `test_basic_account` - Account system OK
2. `test_show_finance_zero` - Bug fix #43 verified
3. `test_errors` - Error handling OK

---

## Verified Working ✅

- Account operations (su, register, logout, delete)
- Login stack and privilege system
- Book select/buy/import
- `modify -price=X.XX` (no quotes needed)
- `show` without filters
- `show -ISBN=XXX`
- show finance (including count=0 fix)
- Error handling

---

## Fix Required

Implement proper quoted string parser that:
1. Detects opening quote after `=`
2. Reads entire content until closing quote (including spaces)
3. Validates no embedded quotes (per spec)
4. Handles all affected commands: modify -name/author/keyword, show -name/author/keyword

**Recommendation**: Parse the line manually, don't rely on stringstream `>>` operator for quoted parameters.

---

## Full Report

See: `REGRESSION_TEST_REPORT.md` (in branch carlos/regression-testing-report)

---

## Recommendation

**DO NOT SUBMIT TO ACMOJ** until this is fixed.

This will fail any test case that:
- Sets book names with spaces (most real books)
- Sets author names with spaces (most real authors)
- Searches by name or author
- Uses keywords

**Priority**: P0 - CRITICAL  
**Blocks**: ACMOJ submission  
**Estimated Fix Time**: 2-3 hours
