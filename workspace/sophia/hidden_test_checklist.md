# Hidden Test (1775) Readiness Checklist

## Based on analysis of 214 visible tests (1075)

---

## ⚠️ CRITICAL - Will Definitely Be Tested

- [ ] **log command** - Currently stub, outputs empty line only
  - Must log: who did what (operation details)
  - Must log: financial transaction details
  - Must log: chronological order
  - Format: self-defined (any reasonable format acceptable)

- [ ] **report finance** - Exists but format unknown
  - Verify output format is "pleasing" and informative
  - Must show financial transaction summary
  - Format: self-defined

- [ ] **report employee** - Exists but format unknown  
  - Verify tracks employee operations
  - Must show what each employee did
  - Format: self-defined

- [ ] **quit command** - Exists, should work same as exit
  - Quick test: does quit terminate properly?
  - Does state save on quit?

---

## 🔴 HIGH PRIORITY - Very Likely Tested

### Whitespace Handling
- [ ] Multiple consecutive spaces treated as single space
- [ ] Leading/trailing spaces allowed and handled
- [ ] Empty lines (only spaces) produce no output
- [ ] Tabs? (not mentioned in spec, probably not used)

### Financial System
- [ ] `show finance 0` outputs empty line
- [ ] `show finance` with Count > total transactions outputs "Invalid"
- [ ] Financial calculations precise (floating point math)
- [ ] Format: `+ [Income] - [Expenditure]\n` exactly

### Invalid Privilege Levels
- [ ] useradd with privilege 0, 2, 4, 5, 6, 8, 9 → "Invalid"
- [ ] useradd where new privilege >= current privilege → "Invalid"
- [ ] Only valid privileges: 1, 3, 7

### String Length Limits
- [ ] UserID, Password: max 30 chars, reject longer
- [ ] Username: max 30 chars
- [ ] ISBN: max 20 chars
- [ ] BookName, Author: max 60 chars
- [ ] Keyword (total): max 60 chars

---

## 🟡 MEDIUM PRIORITY - Likely Tested

### Keyword Edge Cases
- [ ] Modify with duplicate keywords → "Invalid"
- [ ] Show with multiple keywords in search → "Invalid"
- [ ] Empty keyword segments (e.g., "a||b") → "Invalid"

### ISBN Constraints
- [ ] Modify ISBN to same value → "Invalid"
- [ ] Modify ISBN to existing ISBN → "Invalid"
- [ ] ISBN uniqueness maintained

### Numeric Boundaries
- [ ] Quantity max: 2,147,483,647
- [ ] Quantity = 0 for buy/import → "Invalid"
- [ ] Negative quantities → "Invalid"
- [ ] Price = 0.00 (allowed? spec unclear)
- [ ] Very large prices (up to 13 chars)

### Show Command Edge Cases
- [ ] show with empty parameter → "Invalid"
- [ ] show -keyword="multi|keywords" → "Invalid"
- [ ] show with no results → output empty line
- [ ] show with no parameters → output all books

### Modify Command Edge Cases
- [ ] Duplicate parameters → "Invalid"
- [ ] Modify without selection → "Invalid"
- [ ] Modify with all 5 parameters at once (legal)

---

## 🟢 LOW PRIORITY - Possibly Tested

### Login Stack Depth
- [ ] Deep nesting (100+ levels) works
- [ ] Same user multiple simultaneous logins
- [ ] Selected book state across nested logins
- [ ] Logout on empty stack → "Invalid"

### Account Deletion
- [ ] Delete logged-in user → "Invalid"
- [ ] Delete in nested login scenarios
- [ ] Operations after user deleted

### Performance Constraints
- [ ] Memory usage < 64 MiB
- [ ] File count ≤ 20 files
- [ ] Disk usage < 1024 MiB
- [ ] Time < 10000 ms per test

---

## Commands Coverage Summary

| Command | 1075 Tests | Implemented | Quality |
|---------|-----------|-------------|---------|
| su | 49,668 | ✅ | Good |
| logout | 49,313 | ✅ | Good |
| register | 3,145 | ✅ | Good |
| passwd | 20,382 | ✅ | Good |
| useradd | 500 | ✅ | Good |
| delete | 137 | ✅ | Good |
| show | 56,161 | ✅ | Good |
| buy | 107,195 | ✅ | Good |
| select | 14,249 | ✅ | Good |
| modify | 13,438 | ✅ | Good |
| import | 6,181 | ✅ | Good |
| show finance | 354 | ✅ | ? |
| exit | 113 | ✅ | Good |
| **quit** | **0** | ✅ | **?** |
| **log** | **0** | ⚠️ | **STUB** |
| **report finance** | **0** | ✅ | **?** |
| **report employee** | **0** | ✅ | **?** |

---

## Testing Strategy for Hidden Tests

### Before Submission
1. **Manually test all untested commands:**
   - Run log after various operations
   - Run report finance after buy/import
   - Run report employee after useradd/passwd/etc
   - Run quit instead of exit

2. **Test edge cases:**
   - show finance 0
   - show finance 999999999
   - useradd with privilege 2, 4, 5, 6, 8
   - Multiple spaces in commands
   - Maximum length strings

3. **Verify formats:**
   - log output is reasonable
   - report outputs are "pleasing"
   - show finance format matches spec exactly

### Expected Hidden Test Patterns
Based on visible test distribution:

- **20-30% simple tests** (1-100 lines) - edge cases, format verification
- **20-30% medium tests** (100-1000 lines) - combined operations, log testing
- **40-60% stress tests** (1000+ lines) - performance, large datasets

**Key difference from 1075:** Hidden tests will focus on **untested commands and edge cases**, not just stress testing.

---

## Risk Assessment

### HIGH RISK (likely to fail if not fixed)
- ❌ log command stub - almost certain to fail
- ⚠️ report formats unknown - may not match expectations
- ⚠️ Whitespace handling - not verified

### MEDIUM RISK (might fail)
- ⚠️ Invalid privilege levels - not explicitly tested
- ⚠️ String length enforcement - not verified
- ⚠️ Keyword edge cases - partially tested

### LOW RISK (probably okay)
- ✅ Core commands well tested
- ✅ quit command simple to verify
- ✅ Financial calculations likely correct

---

## Recommended Actions

### IMMEDIATE (before any submission)
1. Implement proper `log` command (currently stub)
2. Manually test `report finance` and `report employee` output
3. Test `quit` command (should be trivial)
4. Verify whitespace handling (multiple spaces, leading/trailing)

### IMPORTANT (high value, moderate effort)
5. Test `show finance` edge cases (0, overflow)
6. Verify invalid privilege level rejection
7. Test string length enforcement
8. Test keyword duplicate detection

### NICE TO HAVE (lower priority)
9. Deep login stack testing
10. All numeric boundary tests
11. Performance profiling under constraints
