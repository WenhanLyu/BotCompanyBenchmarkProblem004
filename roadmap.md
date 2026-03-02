# Roadmap for Bookstore Management System (Problem 004)

## Project Goal
Implement a complete bookstore management system in C++ that passes all test cases on ACMOJ (problems 1075 and 1775).

## Current State
- **Phase**: PLANNING (Post-M2.1 completion, ready for M3)
- **Milestone**: M2.1 COMPLETE ✅ (su optional password bug fixed)
- **Repository**: Complete account system with all features, ready for book system
- **Cycles Used**: 26 (3 planning + 5 M1 impl + 2 M1 verify + 1 M2 impl + 2 M2 verify + 7 M2.1 planning + 1 M2.1 impl + 5 M3 planning)

## Strategic Approach

**Vertical Slice Development with Continuous Testing**

- Build working code from Day 1, not infrastructure first
- Test incrementally with available 214 test cases
- Optimize only after profiling proves it necessary
- Prevent regression with automated testing

## Root Milestones (Revised)

### M1: Minimal Viable System ✅ COMPLETE
**Status**: VERIFIED AND PASSED  
**Estimated Cycles**: 5  
**Actual Cycles**: 5 implementation + 2 verification = 7 total  
**Description**: Get basic working system that compiles and passes simplest tests.

**Deliverables**:
- CMakeLists.txt and Makefile that produce `code` executable
- .gitignore configured for OJ submission
- Basic command parser (reads stdin, handles quit/exit)
- Root account initialization (username: root, password: sjtu, privilege: 7)
- Minimal su/logout for root account only
- Simple in-memory storage (file persistence deferred to M2)

**Test Strategy**:
- Test against simplest cases (e.g., data/004/1075/1.in)
- Verify build system works
- Establish testing workflow

**Acceptance Criteria**:
- `cmake . && make` produces `code` executable
- Program reads commands from stdin
- Handles quit/exit to terminate
- Root account auto-created on first run
- Passes 5-10 simplest test cases
- All code committed and pushed

**Why Start Here**: 
- Validates build system immediately
- Proves we can read input and produce output
- Establishes feedback loop early
- Low risk, quick win to build momentum

---

### M2: Account System Core + File Persistence ✅ COMPLETE
**Status**: VERIFIED AND PASSED (with bug discovered post-verification)  
**Estimated Cycles**: 12  
**Actual Cycles**: 1 implementation + 2 verification = 3 total  
**Description**: Complete account system with all commands and file-based persistence.

**Bug Discovered** (Cycle 19 by Nina):
- su command does not implement optional password feature
- Specification requires: "If current privilege > target privilege, password can be omitted"
- Current implementation always requires password
- Affects test 210 and likely others

---

### M2.1: Fix su Optional Password Bug ✅ COMPLETE
**Status**: VERIFIED AND PASSED  
**Estimated Cycles**: 2  
**Actual Cycles**: 1 implementation  
**Description**: Fix critical bug in su command to support optional password per specification.

**Deliverables**:
- Modified su() to allow password omission when current privilege > target privilege
- Added getAccountPrivilege() helper to AccountSystem
- Updated main.cpp su command parsing to accept 1 or 2 parameters
- Test 210 now passes (reduced Invalid count from 6 to 4)

**Result**: Bug fixed, merged to master in commit 54f7ec1

**Key Risks Identified** (Marcus, Cycle 10):
- Login stack refactoring needed (M1 uses single login, not stack)
- File I/O infrastructure starts from zero
- Input validation complexity underestimated
- Edge case handling (Test 3 has 56 lines of errors)

**Deliverables**:
- All account commands: su, logout, register, passwd, useradd, delete
- Login stack implementation (nested logins, same account multiple times)
- Privilege checking (0, 1, 3, 7)
- File-based account storage (simple append/scan approach, NOT B+ tree yet)
- Selected book tracking per login session

**Test Strategy**:
- Test against account-focused test cases
- Verify privilege enforcement
- Test login stack edge cases (delete logged-in account, nested sessions)

**Acceptance Criteria**:
- All account commands work correctly
- Privilege system enforces access control
- Login stack handles nesting and duplicates
- Account data persists across runs
- Passes 20-30 account-focused test cases

**Why This Order**:
- Account system is independent of books
- Learn file I/O requirements from actual usage
- Can validate with subset of tests
- Understand performance needs before complex data structures

---

### M3: Complete Book Management System (Merged M3+M4)
**Status**: NEXT - Ready to define  
**Estimated Cycles**: 22  
**Description**: Implement all 5 book commands with file persistence and multi-attribute search. This is the critical path - 98% of tests require book system.

**Strategic Decision** (Cycle 26):
- Original M3/M4 split was artificial - they describe same interdependent work
- Show command REQUIRES multi-index search from day 1 (cannot defer to M5)
- All 5 book commands are tightly coupled
- Better one coherent milestone than two confused ones

**Deliverables**:
1. Book data structure with all fields (ISBN, name, author, keyword, price, quantity)
2. File-based book storage (books.dat)
3. Commands (all 5):
   - select [ISBN] - select/create book, track in login stack
   - modify [params] - update book attributes (including ISBN change)
   - import [quantity] [cost] - add inventory
   - show [filter?] - query with lexicographic sorting by ISBN
   - buy [ISBN] [quantity] - purchase, update inventory
4. Multi-attribute search indices:
   - ISBN index (primary, unique)
   - Name index (allows duplicates)
   - Author index (allows duplicates)
   - Keyword index (segment-level, allows duplicates)
5. Keyword parsing (pipe-separated `|`, validate segment uniqueness)
6. ISBN modification with transactional index updates
7. Integration with login stack (selected book per session)
8. Comprehensive validation for all edge cases
9. File persistence with efficient I/O

**Test Strategy**:
- Start with tests 11-20 (basic show commands)
- Test 10 (1000 select/modify operations - stress test)
- Mixed workflow tests (110, 150)
- Target: 80-100 tests passing (all tests not requiring log system)

**Acceptance Criteria**:
- All 5 book commands working correctly
- File persistence for book data (survives restart)
- Multi-attribute search returns correct results
- Lexicographic sorting by ISBN (show command)
- Keyword parsing with segment validation (no duplicates)
- ISBN modification updates all indices atomically
- Selected book tracked per login session in stack
- Pass 80-100 tests (target: all non-log tests)
- No regression on account system tests (27/27 still passing)
- Handles 1000+ operations efficiently (Test 10 completes)
- Buy command transaction handling (simple log for now)

**Why 22 cycles**:
- 5 complex commands (not simple CRUD)
- Multi-index implementation required (4 indices minimum)
- Keyword parsing with segment-level validation and indexing
- ISBN modification edge cases (update all indices, prevent duplicates)
- Integration with existing login stack
- Based on Diana's analysis: 20-26 cycles realistic (taking middle estimate)

**Critical Insights** (from team analysis):
- 98.1% of tests (205/209) require book system - this is THE bottleneck
- Buy command is most frequent (107K uses, 33% of ALL operations)
- Book commands: 196K occurrences (61.4% of all commands)
- Must build for scale from day 1 - cannot "defer optimization"

**Risk Mitigation**:
- Build data structures first, then add commands incrementally
- Test after each command (don't wait for all 5)
- Reuse account system's file I/O pattern
- LoginSession already has selectedBook field (no refactoring needed)

**Tricky Parts Identified**:
- Show command lexicographic sorting (must be exact)
- Keyword segment parsing and duplicate detection
- ISBN modification (must maintain index consistency)
- Multi-index updates must be atomic (all or nothing)
- Float precision (2 decimal places for price/cost)

---

### M4: Log System (formerly M6)
**Status**: Planned  
**Estimated Cycles**: 6  
**Description**: Implement logging and financial tracking features.

**Note**: Renumbered from M6 due to M3/M4 merge. M5 (Performance Optimization) removed - optimization built into M3.

---

### M5: Edge Cases & Final Polish (formerly M7)
**Status**: Planned  
**Estimated Cycles**: 8  
**Description**: Handle all edge cases and prepare for hidden tests (1775).

**Deliverables**:
- Profile test case performance (especially stress tests 170-214)
- Implement B+ tree indexes IF tests are too slow
- Multi-index support for efficient search (ISBN, name, author, keyword)
- Stay within 20-file limit
- Optimize memory usage (≤64MB)

**Test Strategy**:
- Run all large test cases (5K-7K lines)
- Measure execution time
- Profile bottlenecks before optimizing

**Acceptance Criteria**:
- All tests pass within 10-second time limit
- Memory usage ≤64MB
- Uses ≤20 files
- Passes 70-80 test cases (including stress tests)

**Decision Point**:
- If simple approach is fast enough, skip complex optimizations
- Only add B+ trees if profiling proves necessary
- Use Liam's architecture design as reference if optimization needed

---

### M6: Log System
**Status**: Planned  
**Estimated Cycles**: 6  
**Description**: Implement logging and financial tracking features.

**Deliverables**:
- Commands: show finance, log, report finance, report employee
- Transaction recording (income/expenditure)
- Employee operation logging
- Query last N transactions

**Test Strategy**:
- Test against log-focused test cases
- Verify financial calculations
- Test report formats

**Acceptance Criteria**:
- All log commands work correctly
- Financial tracking accurate
- Reports format matches specification
- Passes 90+ test cases

**Why Later**:
- Log system can be added independently
- Doesn't affect core account/book functionality
- Lower complexity than book system

---

### M7: Edge Cases & Hidden Test Preparation
**Status**: Planned  
**Estimated Cycles**: 8  
**Description**: Handle all edge cases and prepare for hidden tests (1775).

**Deliverables**:
- Test against ALL 214 test cases
- Fix any remaining bugs
- Handle edge cases:
  - Empty files (tests 135-139)
  - Error injection (test 3)
  - Boundary conditions
  - Invalid commands
- Exact output format verification (floating point precision, tabs)

**Test Strategy**:
- Run complete test suite
- Identify failing tests and fix systematically
- Test error handling thoroughly

**Acceptance Criteria**:
- Passes all 214 test cases in data/004/1075/
- No memory leaks
- Output format exactly matches requirements
- Handles all error cases correctly

**Hidden Test Considerations**:
- Problem 1775 has unknown test cases
- Likely tests edge cases harder
- Ensure robust error handling

---

### M8: Final Verification & Submission Preparation
**Status**: Planned  
**Estimated Cycles**: 5  
**Description**: Final verification, code cleanup, and mark project complete.

**Deliverables**:
- Final round of testing (all 214 cases)
- Code review and cleanup
- Remove debug code
- Verify git repository ready for submission
- Final documentation
- Mark project complete for external evaluation

**Test Strategy**:
- Full regression test
- Clean build verification
- Submission dry-run

**Acceptance Criteria**:
- All tests pass consistently (multiple runs)
- Code is clean and well-organized
- No compilation warnings
- Git repository properly configured
- Ready for ACMOJ submission

**Completion Criteria**:
- Project marked complete
- External evaluation runner handles submission
- Team waits for ACMOJ feedback

---

## Total Estimated Cycles: 57
- M1: 7 actual (5 estimated)
- M2: 3 actual (12 estimated) 
- M2.1: 1 actual (2 estimated)
- M3 (merged M3+M4): 22 estimated (was 18 split)
- M4 (formerly M6): 6 estimated
- M5 (formerly M7): 8 estimated
- M6 (formerly M8): 5 estimated
- Planning cycles used: 5 (cycles 1-3, 10, 19-26)

## Lessons Learned

### Cycle 1-3 (Planning Phase)
- **✅ Good**: Thorough analysis by specialized workers (Emma, Sophia, Liam, Diana, Marcus)
- **✅ Good**: Independent blind evaluation revealed critical issues
- **⚠️ Adjusted**: Original waterfall approach (M1-M7, 53 cycles) was too risky
- **✅ Pivoted**: To vertical slice development with continuous testing
- **📊 Key Insight**: Don't build infrastructure before validating requirements with real code
- **📊 Key Insight**: 214 available test cases are invaluable - use them from Day 1

### Cycle 4-9 (M1 Implementation)
- **✅ Success**: M1 completed in exactly 5 cycles as estimated
- **✅ Success**: 27/27 tests passed (far exceeded 5-10 target)
- **✅ Good**: Build system, command parser, basic account system all working
- **⚠️ Limitation**: Login stack is single-user (must refactor for M2)
- **📊 Key Insight**: Vertical slice approach works - test from Day 1, iterate quickly
- **📊 Key Insight**: Apollo's verification (2 cycles) caught the limitation but M1 still passes

### Cycle 10 (M2 Planning)
- **✅ Good**: Emma created comprehensive M2 requirements (23KB document)
- **✅ Good**: Marcus identified M2 underestimated by 40-50% (8→12 cycles)
- **⚠️ Risk**: Login stack refactoring is architectural change, not feature add
- **📊 Key Insight**: Better to overestimate cycles than rush and fail verification

### Cycle 11-18 (M2 Implementation & Verification)
- **✅ Success**: M2 completed in 1 cycle (Noah's exceptional work)
- **✅ Success**: Apollo verification passed, 27/27 tests
- **⚠️ Bug Found**: Su optional password not implemented (post-verification)
- **📊 Key Insight**: Even thorough verification can miss spec details

### Cycle 19-25 (M2.1 Bug Fix + M3 Planning)
- **✅ Success**: Su bug fixed in 1 cycle (Leo)
- **✅ Good**: Comprehensive team analysis (Emma, Sophia, Diana, Marcus, Liam)
- **✅ Critical**: Diana identified M3 underestimated by 100% (10→20-26 cycles)
- **✅ Critical**: Sophia's test analysis - 98% of tests need book system
- **📊 Key Insight**: Artificial milestone splits create confusion - merge related work
- **📊 Key Insight**: Cannot "defer optimization" when feature requires indices from day 1
- **📊 Key Decision**: Merged M3+M4 into single coherent milestone (22 cycles)

### Strategic Decisions
1. **Start simple, iterate**: In-memory → simple files → optimized indexes (only if needed)
2. **Test continuously**: Each milestone validates with subset of test cases
3. **Defer optimization**: Profile before adding B+ trees
4. **Use Liam's design as reference**: Excellent architecture doc, but don't over-engineer upfront

### Risk Mitigation
- **Regression prevention**: Test suite runs each milestone
- **Early validation**: Milestone 1 proves build system works
- **Incremental complexity**: Each milestone adds working features
- **Flexibility**: Can adjust approach based on actual performance data

---

## Constraints & Requirements

### Technical Constraints
- **Language**: C++ only
- **Compilation**: CMake + Make → produces `code` executable
- **Files**: Maximum 20 files total
- **Time Limit**: 10 seconds per test case
- **Memory Limit**: 64 MB per test case
- **Persistence**: File-based storage required (not in-memory only)

### Critical Requirements
- Root account auto-created: username `root`, password `sjtu`, privilege `7`
- Privilege levels: Only 0, 1, 3, 7 are valid
- Login stack: Nested logins supported, same account multiple times
- Selected book: Persists per login session in the stack
- Exact output format: Floating point precision, tab separation
- Error handling: Output "Invalid\n" for illegal commands

### Test Data
- **Available**: 214 test cases in data/004/1075/ (problem 1075)
- **Hidden**: Unknown test cases for problem 1775
- **Range**: 29 lines to 7,606 lines per test
- **Categories**: Basic, search, edge cases, workflows, stress tests

### Submission
- **ACMOJ Problem IDs**: 1075, 1775
- **Max Submissions**: 8 counted submissions
- **Repository URL**: https://github.com/WenhanLyu/BotCompanyBenchmarkProblem004.git
- **External evaluation**: TBC agents don't submit - external runner handles it

---

## Next Actions

1. **Athena**: Define M1 milestone and transition to Ares
2. **Ares**: Build M1 (minimal viable system)
3. **Apollo**: Verify M1 meets acceptance criteria
4. **Iterate**: M2 → M3 → ... → M8

---

**Last Updated**: Cycle 3 (Athena)  
**Status**: Ready to define M1 and hand off to Ares
