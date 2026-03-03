# Roadmap for Bookstore Management System (Problem 004)

## Project Goal
Implement a complete bookstore management system in C++ that passes all test cases on ACMOJ (problems 1075 and 1775).

## Current State (Cycle 91)
- **Phase**: PLANNING (post-M5 independent evaluation)
- **Repository**: M5 fixes applied but NEW bugs discovered in evaluation
  - Problem 1075: 93/100 (only 2 failures: testpoints 3, 212)
  - Problem 1775: 0/100 (COMPLETE FAILURE - hidden tests)
- **Submissions Used**: 1/8
- **Status**: Three additional critical bugs found in blind evaluation
- **Cycles Used**: 91 total

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
- M5: Fix OJ Submission #1 Failures ✅ COMPLETE (but more bugs found)
- **M5.1: Fix Remaining Edge Case Validation Bugs** ← CURRENT

## M5: Fix OJ Submission #1 Failures ✅ COMPLETE

**Status**: COMPLETE (Cycles 83-89)  
**Actual Cycles**: 7 (Investigation: 7, Implementation: 2, Verification: 1)

**Bugs Fixed**:
1. ✅ BUG #1: Whitespace-only lines (Leo, commit 477f6b0)
2. ✅ BUG #2: Extra arguments validation (Maya, commit 9fa9d41)
3. ✅ BUG #3: passwd 3-arg form (Alice, commit caea411)
4. ✅ BUG #4: Empty parameter validation (Maya, commit 10f75f4)
5. ✅ BUG #5: Empty keyword trailing pipe (Diana, commit 2173930)

**Result**: Passed Apollo verification, but Athena's blind evaluation (Cycle 91) found additional bugs

---

## M5.1: Fix Remaining Edge Case Validation Bugs

**Status**: Ready for IMPLEMENTATION  
**Estimated Cycles**: 2-3  
**Description**: Fix newly discovered edge case validation bugs found in blind evaluation

**Bugs Found in Cycle 91 Evaluation**:

**CRITICAL BUGS** (Must fix before submission):
1. ❌ **quit/exit don't validate extra arguments** (Bob)
   - Location: main.cpp lines 74-76
   - Issue: `quit extra` exits without outputting "Invalid"
   - Same for `exit extra`
   - Impact: Basic validation gap, high probability in hidden tests
   - Fix time: 5 minutes

2. ❌ **show accepts multiple parameters** (Gordon)
   - Location: main.cpp lines 540-610
   - Issue: `show -ISBN=A -name="B"` should return "Invalid" per spec R71
   - Currently succeeds
   - Impact: HIGH - clear spec violation
   - Fix time: 15 minutes

**HIGH PRIORITY** (Should verify):
3. ⚠️ **show finance 0 logic inconsistency** (Vera)
   - main.cpp says output empty line, book.cpp comment says "include all"
   - Need to verify spec requirement and align
   - Fix time: 10-30 minutes

**MEDIUM PRIORITY** (Optional verification):
4. ⚠️ **su password verification when not required** (Vera)
   - When user has higher privilege, wrong password still causes failure
   - Verify if this is intended per spec
   - Fix time: 10 minutes if needed

**Acceptance Criteria**:
- quit/exit validate extra arguments correctly
- show rejects multiple parameters
- show finance 0 behavior verified and correct
- Comprehensive validation sweep completed
- All previous tests still pass
- Ready for OJ submission #2 with high confidence

**Why This Milestone**:
- M5 fixes were incomplete - missed quit/exit in extra args validation
- Gordon independently found show multi-param bug
- Pattern recognition: If we missed 2-3 obvious bugs, likely more exist
- Better to fix KNOWN bugs now than waste submissions on them
- All fixes are simple (< 1 hour total implementation time)

**Budget Awareness**:
- Submissions remaining: 7/8
- Use submissions for UNKNOWN issues, not KNOWN bugs
- Target after M5.1: 100/100 on 1075, 95+/100 on 1775

---

## Lessons Learned

### Cycle 91 (Post-M5 Blind Evaluation)
- **⚠️ Critical**: Fixes can be INCOMPLETE - M5 fixed "extra args" but missed quit/exit
- **📊 Key Insight**: Independent blind evaluation AFTER verification is essential
- **✅ Good**: Multiple blind evaluators (Gordon, Bob, Emma, Diana, Vera) found 3 new bugs
- **⚠️ Pattern**: If we miss obvious bugs in a fix, there are likely MORE bugs
- **📊 Decision Framework**: Found bugs = Do NOT submit yet, fix known issues first
- **✅ Strategy**: Use OJ submissions for UNKNOWN issues, not KNOWN bugs

### Cycle 76-83 (OJ Submission #1 Failure Investigation)
- **✅ Good**: 93/100 on 1075 confirms core logic is sound
- **⚠️ Critical**: 0/100 on 1775 reveals systematic edge case validation gaps
- **📊 Key Insight**: Hidden tests (1775) test edge cases not covered by public tests (1075)
- **📊 Key Insight**: Blind-mode independent investigation found root causes efficiently
- **⚠️ Bugs Found**: 5 total - all fixed, but fixes were incomplete
- **✅ Strategy**: Independent testing by Athena confirmed worker findings

### Previous Cycles Summary
- Vertical slice development worked well
- Independent blind evaluation caught critical bugs
- Comprehensive testing before submission saved cycles
- But edge case validation requires multiple passes to catch everything

---

## Next Actions

1. **Athena (Cycle 91)**: 
   - ✅ M5 completed but found insufficient in blind evaluation
   - ✅ Three new critical bugs identified
   - Ready to hand off M5.1 to Ares
   
2. **Ares Team (M5.1)**: 
   - Fix quit/exit extra argument validation (CRITICAL)
   - Fix show multi-parameter validation (CRITICAL)
   - Verify show finance 0 behavior (HIGH)
   - Optional: Verify su password check behavior
   - Comprehensive validation sweep
   - Test thoroughly

3. **After M5.1**: OJ submission #2 with 95%+ confidence

---

**Last Updated**: Cycle 91 (Athena)  
**Status**: M5.1 ready for implementation, 3 bugs identified with fixes specified
