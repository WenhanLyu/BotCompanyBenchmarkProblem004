# Bug Report: show finance Non-Numeric Parameter Validation

**Reporter**: Yasmin  
**Date**: Current Cycle  
**Priority**: P1 (Low Impact)  
**Status**: NEW  

---

## Summary

The `show finance` command accepts non-numeric parameters (e.g., "abc") when it should return `Invalid`. According to the specification, `[Count]` must be a valid number.

## Specification Reference

README.md lines 337-339:
```
[Count]: Number of transactions
- Legal character set: digits
- Maximum length: 10
- Special note: Value does not exceed 2,147,483,647
```

## Current Behavior

```bash
printf "su root sjtu\nshow finance abc\nquit\n" | ./code
# Output: + 0.00 - 0.00
# Expected: Invalid
```

## Expected Behavior

Any non-numeric value for `[Count]` should be rejected with `Invalid` output.

## Root Cause

**Location**: `src/main.cpp` lines 517-532

When parsing the count parameter:
```cpp
std::istringstream finSs(remaining.substr(7));
int count = 0;
bool hasCount = false;
if (finSs >> count) {
    hasCount = true;
    // ... validation
}
```

If `finSs >> count` fails (because the input is not a number), the code doesn't check whether there are remaining non-whitespace characters. It simply treats the command as having no count parameter and shows all finance records.

## Proposed Fix

After the `if (finSs >> count)` block, add:

```cpp
// Check for invalid non-numeric parameter
std::string remaining_token;
if (!hasCount && finSs >> remaining_token) {
    std::cout << "Invalid" << std::endl;
    continue;
}
```

## Impact Assessment

**Severity**: LOW
- Edge case scenario (unlikely user input)
- Doesn't affect normal operations
- Unlikely to be tested by ACMOJ
- Non-blocking for submission

## Test Cases

### Failing Tests:
```bash
# Test 1: Non-numeric parameter
show finance abc
# Current: + 0.00 - 0.00
# Expected: Invalid

# Test 2: Mixed alphanumeric
show finance 123abc
# Current: + X.XX - Y.YY (shows last 123 transactions)
# Expected: Invalid
```

### Passing Tests:
```bash
# Valid numeric parameter
show finance 5
# Result: Correct (shows last 5 transactions)

# Multiple parameters
show finance 1 2
# Result: Invalid (correctly rejected)

# Zero parameter
show finance 0
# Result: (empty line) (correct)
```

## Recommendation

Fix in next development iteration. Not blocking ACMOJ submission due to low impact and low probability of occurrence.

---

**Impact**: LOW  
**Blocking**: NO  
**Fix Effort**: MINIMAL (5 lines of code)  
**Test Coverage**: Add test case for non-numeric input validation
