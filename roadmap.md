# Roadmap for Bookstore Management System (Problem 004)

## Project Goal
Implement a complete bookstore management system in C++ that passes all test cases on ACMOJ (problems 1075 and 1775).

## Current State (Cycle 104)
- **Phase**: PLANNING (post-M5.1.1 completion, regression found)
- **Repository**: M5.1 fixes ALL MERGED to master
  - BUG #1 fix (quit/exit validation): MERGED but HAS REGRESSION
  - BUG #2 fix (show multi-param): MERGED and working ✓
  - BUG #3 fix (finance comment): MERGED and working ✓
- **Problem Status**:
  - Problem 1075: 93/100 (only 2 failures: testpoints 3, 212)
  - Problem 1775: 0/100 (COMPLETE FAILURE - hidden tests)
- **Submissions Used**: 1/8
- **Status**: Critical regression found in quit/exit validation
- **Cycles Used**: 104 total

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
- M5.1: Fix Remaining Edge Case Validation Bugs ⚠️ INCOMPLETE (deadline missed)
- **M5.1.1: Merge and Verify M5.1 Fixes** ← CURRENT

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

**Status**: INCOMPLETE (deadline missed - Ares used 3/3 cycles)  
**Estimated Cycles**: 3 (used)
**Actual Result**: Implementation completed but NOT merged to master
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

**What Was Completed (Cycles 92-94)**:
1. ✅ BUG #1 fix created: leo/fix-quit-exit-validation branch, PR #5
2. ✅ BUG #2 fix created: maya/fix-show-multi-params-v2 branch, PR #6
3. ✅ BUG #3 fix created: noah/fix-finance-comment branch (comment update only)
4. ✅ All fixes tested individually and work correctly

**What Was NOT Completed**:
1. ❌ PRs not merged to master
2. ❌ No integration testing after merging all fixes
3. ❌ No final verification on master

**Root Cause**: Milestone underestimated the merge+verify workflow overhead

**Acceptance Criteria** (original):
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

## M5.1.1: Merge and Verify M5.1 Fixes ✅ COMPLETE (with regression)

**Status**: COMPLETE but REGRESSION FOUND (Cycles 95-103)  
**Estimated Cycles**: 2 (used 9 cycles - 2 for merge, 7 for verification)

**What Was Completed**:
1. ✅ Merged PR #5 (Leo's quit/exit validation fix) - commit 475219b
2. ✅ Merged Maya's show multi-parameter fix - commit c0dd5cc
3. ✅ Merged Noah's comment fix - commit 0863128
4. ✅ All fixes verified to output "Invalid" correctly
5. ✅ Build succeeds, no compiler warnings

**CRITICAL REGRESSION FOUND (Cycle 103)**:
- Emma's audit discovered quit/exit validation is INCOMPLETE
- Bug: `quit extra` outputs "Invalid" then EXITS program (should continue)
- Root cause: Unconditional `break` statement at line 79
- Fix needed: Move break inside else clause, add continue to validation branch
- Impact: Test cases with `quit extra` followed by more commands will fail

**Test proving regression**:
```
quit extra
su root sjtu
quit
```
Expected: "Invalid", then su succeeds, then quit
Actual: "Invalid" then program exits (never reaches su)

**Why Regression Occurred**:
- Original M5.1 BUG #1 fix added validation check correctly
- But control flow was wrong - break executes even when extra args present
- Code review missed this during merge
- Independent verification (Emma, cycle 103) caught it

**Lessons Learned**:
- Validation logic ≠ complete fix (must consider control flow)
- Code review should verify control flow, not just logic
- Independent verification after merge is essential

---

## M5.1.2: Fix quit/exit Control Flow Regression

**Status**: Ready for IMPLEMENTATION  
**Estimated Cycles**: 1
**Description**: Fix critical regression in quit/exit validation where program exits instead of continuing after outputting "Invalid"

**The Bug**:
```cpp
// CURRENT (BROKEN):
if (command == "quit" || command == "exit") {
    std::string extra;
    if (ss >> extra) {
        std::cout << "Invalid" << std::endl;
    }
    break;  // ← BUG: Always breaks, even when extra args present!
}
```

**The Fix**:
```cpp
// CORRECT:
if (command == "quit" || command == "exit") {
    std::string extra;
    if (ss >> extra) {
        std::cout << "Invalid" << std::endl;
        continue;  // ← Continue reading commands
    }
    break;  // Only break if no extra args
}
```

**Location**: src/main.cpp lines 74-79

**Test Case**:
```
Input:
quit extra
su root sjtu
quit

Expected Output:
Invalid
(no output from su - succeeds)
(program exits on second quit)

Current (BROKEN) Output:
Invalid
(program exits immediately - never reaches su)
```

**Acceptance Criteria**:
- `quit extra` outputs "Invalid" and continues ✓
- `exit extra` outputs "Invalid" and continues ✓
- `quit` (no args) exits cleanly ✓
- `exit` (no args) exits cleanly ✓
- Test case above passes completely
- All existing tests still pass (no regression)
- Build succeeds

**Why This Milestone**:
- This is a 1-line fix but it's CRITICAL for correctness
- The bug was introduced by M5.1 BUG #1 "fix" (PR #5)
- Emma's audit (cycle 103) found the regression
- Any OJ test with `quit extra` followed by more commands will fail
- Must fix before OJ submission #2

**Budget**: 1 cycle is sufficient - this is a trivial code change + testing

---

## Lessons Learned

### Cycle 103 (M5.1.1 Completion - Regression Found)
- **⚠️ CRITICAL**: Validation check ≠ complete fix. Must verify CONTROL FLOW too.
- **🐛 Bug Pattern**: The quit/exit "fix" added validation but wrong break placement
- **✅ Good**: Emma's independent blind audit caught the regression before OJ submission
- **📊 Key Insight**: Code review must verify: (1) logic correct, (2) output correct, (3) control flow correct
- **⚠️ Testing Gap**: Need test cases that verify program CONTINUES after "Invalid" output
- **✅ Strategy**: Always test both success and failure paths for each command
- **📊 Decision**: Fix immediately in M5.1.2 (1 cycle) before any OJ submission

### Cycle 94 (M5.1 Deadline Miss)
- **⚠️ Critical**: Milestones must include "merge to master" as explicit acceptance criteria
- **⚠️ Critical**: Implementation time ≠ completion time. Budget for: code → PR → review → merge → verify
- **📊 Key Insight**: 3 cycles was enough for implementation but not for complete workflow
- **✅ Good**: All fixes were completed and tested individually - quality was high
- **⚠️ Process**: For small fixes, consider reducing PR overhead (one branch for multiple related fixes)
- **📊 Decision**: Create small "completion" milestone (M5.1.1) rather than breaking down or restarting
- **✅ Strategy**: When work is 95% done, finish it before moving on

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

1. **Athena (Cycle 104)**: 
   - ✅ M5.1.1 completed: All 3 fixes merged to master
   - ✅ Emma's audit found critical regression in quit/exit validation
   - ✅ Analyzed bug: Control flow error (unconditional break)
   - ✅ Created M5.1.2 milestone with exact fix specified
   - Ready to hand off M5.1.2 to Ares
   
2. **Ares Team (M5.1.2)**: 
   - Fix src/main.cpp lines 74-79
   - Move break inside else clause
   - Add continue after "Invalid" output
   - Test: `quit extra` followed by more commands
   - Verify all existing tests still pass

3. **After M5.1.2**: Final verification sweep, then OJ submission #2

---

**Last Updated**: Cycle 104 (Athena)  
**Status**: M5.1.2 ready for implementation - 1 critical regression fix needed
