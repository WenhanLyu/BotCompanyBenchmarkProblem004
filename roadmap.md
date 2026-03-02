# Roadmap for Bookstore Management System (Problem 004)

## Project Goal
Implement a complete bookstore management system in C++ that passes all test cases on ACMOJ (problems 1075 and 1775).

## Current State
- **Phase**: PLANNING → IMPLEMENTATION (starting M1)
- **Milestone**: Defining M1 (Minimal Viable System)
- **Repository**: No code yet - documentation and analysis complete
- **Cycles Used**: 3 (planning and analysis)

## Strategic Approach

**Vertical Slice Development with Continuous Testing**

- Build working code from Day 1, not infrastructure first
- Test incrementally with available 214 test cases
- Optimize only after profiling proves it necessary
- Prevent regression with automated testing

## Root Milestones (Revised)

### M1: Minimal Viable System ✅ NEXT
**Status**: Ready to define  
**Estimated Cycles**: 5  
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

### M2: Account System Core + File Persistence
**Status**: Planned  
**Estimated Cycles**: 8  
**Description**: Complete account system with all commands and simple file-based persistence.

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

### M3: Book System Basic
**Status**: Planned  
**Estimated Cycles**: 10  
**Description**: Basic book operations with simple storage and linear search.

**Deliverables**:
- Commands: select, show (all books), buy
- Book file storage (simple, not optimized)
- ISBN uniqueness enforcement
- Inventory management
- Basic show command (no filters yet)

**Test Strategy**:
- Test against basic book test cases
- Verify inventory updates correctly
- Test select/buy interaction

**Acceptance Criteria**:
- Basic book operations work
- Books persist correctly
- Inventory tracking accurate
- Selected book persists in login session
- Passes 30-40 total test cases

**Why This Order**:
- Get books working before optimizing
- Validate requirements understanding
- May discover simple storage is sufficient for basic operations

---

### M4: Advanced Book Features
**Status**: Planned  
**Estimated Cycles**: 8  
**Description**: Complete book system with modify, import, and filtered search.

**Deliverables**:
- modify command with all parameters (-ISBN, -name, -author, -keyword, -price)
- import command
- show with filters (-ISBN, -name, -author, -keyword)
- Keyword parsing (pipe-separated, no duplicates)
- Multi-attribute search (may need simple indexes)

**Test Strategy**:
- Test complex query scenarios
- Test ISBN modification (requires reindexing)
- Test keyword edge cases

**Acceptance Criteria**:
- All book commands work correctly
- Complex queries return correct results
- Modify updates all indexes properly
- Keyword handling correct (parsing, searching)
- Passes 60-70 total test cases

**Risk Areas**:
- Keyword parsing complexity
- ISBN modification requires updating multiple indexes
- Selected book interaction with modify

---

### M5: Performance Optimization
**Status**: Planned  
**Estimated Cycles**: 8  
**Description**: Profile performance and add optimized data structures ONLY IF NEEDED.

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

## Total Estimated Cycles: 58

## Lessons Learned

### Cycle 1-3 (Planning Phase)
- **✅ Good**: Thorough analysis by specialized workers (Emma, Sophia, Liam, Diana, Marcus)
- **✅ Good**: Independent blind evaluation revealed critical issues
- **⚠️ Adjusted**: Original waterfall approach (M1-M7, 53 cycles) was too risky
- **✅ Pivoted**: To vertical slice development with continuous testing
- **📊 Key Insight**: Don't build infrastructure before validating requirements with real code
- **📊 Key Insight**: 214 available test cases are invaluable - use them from Day 1

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
