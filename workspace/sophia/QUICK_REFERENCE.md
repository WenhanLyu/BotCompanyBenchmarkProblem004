# Quick Reference - Hidden Test Gaps

## 🔴 CRITICAL - Fix Immediately

### Log Command (STUB!)
```cpp
// Current implementation in src/main.cpp:
if (command == "log") {
    std::cout << std::endl;  // ← STUB! Just outputs empty line
    continue;
}
```
**Problem:** Hidden tests expect actual operation logging  
**Fix:** Log all operations (who, what, when) in reasonable format

### Report Commands
- `report finance` - Exists but format unverified
- `report employee` - Exists but format unverified  
**Action:** Manually test outputs, ensure "pleasing" format

---

## ❌ NOT TESTED in 1075 (0 occurrences)

| Command | Count | Status |
|---------|-------|--------|
| `log` | 0 | ⚠️ STUB |
| `report finance` | 0 | ✅ Exists |
| `report employee` | 0 | ✅ Exists |
| `quit` | 0 | ✅ Exists |

**All four will be in hidden tests!**

---

## ⚠️ Edge Cases NOT Tested

### Whitespace
- Multiple consecutive spaces → single space
- Leading/trailing spaces allowed
- Empty lines (spaces only) → no output

### Financial
- `show finance 0` → empty line
- `show finance <too large>` → "Invalid"
- Format: `+ [Income] - [Expenditure]\n`

### Invalid Values
- Privileges: only 1, 3, 7 valid (reject 0,2,4,5,6,8,9)
- String lengths: 30/60/20 char limits
- Quantities: 1 to 2,147,483,647 only

### Keywords
- Duplicate keywords → "Invalid"
- Multiple keywords in search → "Invalid"
- Empty segments (a||b) → "Invalid"

---

## ✅ Well Tested (Probably OK)

- Account operations (49K logins)
- Book operations (107K buys, 13K modifies)
- Show queries (56K)
- Stress tests (up to 7620 lines)
- Error handling (Invalid output)

---

## Command Frequency

```
buy:      107,195  (51% of all commands)
show:      56,161  (27%)
su:        49,668  (24%)
logout:    49,313  (24%)
passwd:    20,382  (10%)
select:    14,249  (7%)
modify:    13,438  (6%)
import:     6,181  (3%)
register:   3,145  (2%)
useradd:      500  (<1%)
delete:       137  (<1%)
exit:         113  (<1%)
---
log:            0  ← NOT TESTED
quit:           0  ← NOT TESTED
report *:       0  ← NOT TESTED
```

---

## Test Size Breakdown

```
0 lines:       5 tests
5-9 lines:   103 tests  ← Majority
10-99:         9 tests
100-499:      20 tests
500-999:      25 tests
1000-4999:    11 tests
5000-7499:    26 tests
7500+:        15 tests  ← Max stress
```

---

## Pre-Submission Checklist

- [ ] Test `log` command (currently stub!)
- [ ] Test `report finance` output
- [ ] Test `report employee` output
- [ ] Test `quit` command (vs exit)
- [ ] Test `show finance 0`
- [ ] Test `show finance 999999`
- [ ] Test multiple spaces in commands
- [ ] Test invalid privilege levels (2,4,5,6,8)
- [ ] Test max length strings
- [ ] Test duplicate keywords

**If all checked:** ~85-95% chance of passing 1775  
**If log still stub:** ~40-60% chance of passing 1775

---

See `test_analysis.md` for full details.
