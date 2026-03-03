# Roadmap for Bookstore Management System (Problem 004)

## Project Goal
Implement a complete bookstore management system in C++ that passes all test cases on ACMOJ (problems 1075 and 1775).

## Current State (Cycle 83)
- **Phase**: PLANNING → Ready to transition to IMPLEMENTATION
- **Repository**: First ACMOJ submission failed
  - Problem 1075: 93/100 (only 2 failures: testpoints 3, 212)
  - Problem 1775: 0/100 (COMPLETE FAILURE - hidden tests)
- **Submissions Used**: 1/8
- **Root Cause Identified**: TWO critical parsing bugs in main.cpp
- **Cycles Used**: 83 total

## OJ Submission #1 Analysis

**Problem 1075 Results**: 93/100 (Excellent)
- Pass rate: 212/214 testpoints (99.1%)
- Only 2 failures: testpoint 3 (BasicTestCase-3), testpoint 212 (RobustTestCase-3)
- Suggests core implementation is sound

**Problem 1775 Results**: 0/100 (CRITICAL FAILURE)
- Testpoints 1-5: PASS ✓
- Testpoint 6: FAIL (InnerTestCase-1) → breaks subtask
- Testpoint 7: FAIL (InnerTestCase-2) → skips 8-12
- Testpoint 13: FAIL (InnerTestCase-3) → skips 14-17
- Testpoint 19: FAIL (InnerTestCase-4) → skips 20-21
- Pattern suggests systematic edge case handling issue

**Root Cause CONFIRMED (Cycle 83)**:
1. ✅ Whitespace-only lines output "Invalid" instead of no output
2. ✅ Extra arguments silently ignored instead of returning "Invalid"
3. ✅ Empty parameter validation (fixed in cycle 77)
4. ✅ passwd 3-arg form for {7} (fixed in cycle 81)
5. ✅ Empty keyword trailing pipe (fixed in cycle 67)

## Bugs Identified (Cycles 76-83)

**BUG #1: Whitespace-only lines output "Invalid"** ← CRITICAL
- Location: src/main.cpp lines 59-61
- Spec: Lines with only spaces should produce NO output
- Current: `echo "   " | ./code` outputs "Invalid"
- Fix: Check if line contains only whitespace before parsing
- Impact: Very High - OJ systems frequently test whitespace edge cases
- Status: Ready to fix

**BUG #2: Extra arguments silently ignored** ← CRITICAL
- Location: src/main.cpp (su, logout, delete, select, buy, import, log, report)
- Spec: Commands with wrong arg count should return "Invalid"
- Current: `echo "su root sjtu extra" | ./code` succeeds
- Fix: Check for extra arguments after expected params
- Impact: Very High - malformed input should be rejected
- Status: Ready to fix

**BUG #3: passwd 3-arg form for {7}** ✅ FIXED (cycle 81, commit caea411)
**BUG #4: Empty parameter validation** ✅ FIXED (cycle 77, commit 10f75f4)
**BUG #5: Empty keyword trailing pipe** ✅ FIXED (cycle 67, commit 2173930)

## Root Milestones (Summary)

- M1: Minimal Viable System ✅ COMPLETE
- M2: Account System Core + File Persistence ✅ COMPLETE
- M2.1: Fix su Optional Password Bug ✅ COMPLETE
- M3: Complete Book Management System ✅ COMPLETE
- M4: Fix Critical Bugs ✅ COMPLETE
- M4.1: Fix Quoted String Parsing Bug ✅ COMPLETE
- **M5: Fix OJ Submission #1 Failures** ← CURRENT

## M5: Fix OJ Submission #1 Failures

**Status**: Investigation COMPLETE → Ready for IMPLEMENTATION  
**Estimated Cycles**: 2-3  
**Description**: Fix two critical parsing bugs causing 1775 failures

**Investigation Complete** (Cycles 76-83):
1. ✅ Review OJ results and identify failure pattern (Athena)
2. ✅ Analyze spec for validation requirements (Gordon, Bob)
3. ✅ Test empty parameter validation (Maya - FIXED)
4. ✅ Test passwd 3-arg form (Alice - FIXED)
5. ✅ Test keyword validation (Diana - FIXED)
6. ✅ Systematic edge case audit (Gordon - FOUND 2 CRITICAL BUGS)
7. ✅ Character set/length validation (Bob - 100% compliant)
8. ✅ Build and test current code (Emma - all commands work)

**Bugs to Fix**:
1. ✅ BUG #3: passwd 3-arg form (Alice, commit caea411)
2. ✅ BUG #4: Empty parameter validation (Maya, commit 10f75f4)
3. ✅ BUG #5: Empty keyword trailing pipe (Diana, commit 2173930)
4. ⏳ BUG #1: Whitespace-only lines output "Invalid" ← NEXT
5. ⏳ BUG #2: Extra arguments silently ignored ← NEXT

**Acceptance Criteria**:
- BUG #1 fixed: Whitespace-only lines produce no output
- BUG #2 fixed: Extra arguments cause "Invalid"
- All edge cases tested
- No regression in existing tests
- Ready for OJ submission #2

**Budget Awareness**:
- Submissions remaining: 7/8
- High confidence fix - only 2 remaining bugs, both confirmed
- Target: 100/100 on 1075, 90+/100 on 1775

---

## Lessons Learned

### Cycle 76-83 (OJ Submission #1 Failure Investigation)
- **✅ Good**: 93/100 on 1075 confirms core logic is sound
- **⚠️ Critical**: 0/100 on 1775 reveals systematic edge case validation gaps
- **📊 Key Insight**: Hidden tests (1775) test edge cases not covered by public tests (1075)
- **📊 Key Insight**: Blind-mode independent investigation found root causes efficiently
- **⚠️ Bugs Found**: 5 total - 3 already fixed, 2 remaining (whitespace, extra args)
- **📊 Key Decision**: Thorough investigation paid off - high confidence in fix
- **✅ Strategy**: Independent testing by Athena confirmed worker findings

### Previous Cycles Summary
- Vertical slice development worked well
- Independent blind evaluation caught critical bugs
- Comprehensive testing before submission saved cycles
- But missed edge case validation requirements

---

## Next Actions

1. **Athena (Cycle 83)**: 
   - ✅ Investigation complete
   - ✅ Root causes identified and confirmed
   - Ready to hand off M5 to Ares
   
2. **Ares Team**: 
   - Fix BUG #1: Whitespace-only line handling
   - Fix BUG #2: Extra argument detection
   - Test thoroughly
   - Prepare for OJ submission #2

3. **Target**: 100/100 on 1075, 90+/100 on 1775

---

**Last Updated**: Cycle 83 (Athena)  
**Status**: Investigation complete, ready to implement fixes
