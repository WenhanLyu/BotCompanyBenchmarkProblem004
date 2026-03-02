# Roadmap for Bookstore Management System (Problem 004)

## Project Goal
Implement a complete bookstore management system in C++ that passes all test cases on ACMOJ (problems 1075 and 1775).

## Current State
- **Phase**: PLANNING (Initial)
- **Milestone**: None yet
- **Repository**: Clean state, no commits yet

## Root Milestones

### M1: Project Infrastructure & Build System
**Status**: Planned  
**Estimated Cycles**: 3  
**Description**: Set up the basic project structure, build system, and development environment.
- Create CMakeLists.txt for compilation
- Create Makefile
- Create .gitignore
- Set up basic file structure
- Ensure compilation produces `code` executable

**Acceptance Criteria**:
- `cmake .` runs successfully
- `make` produces a `code` executable
- Basic "hello world" program compiles and runs

---

### M2: Core Data Structures & File I/O Foundation
**Status**: Planned  
**Estimated Cycles**: 8  
**Description**: Implement the fundamental data structures and file I/O mechanisms required for persistent storage.
- Block-based file storage system
- B+ tree or similar index structure for efficient lookups
- Account storage and retrieval
- Book storage and retrieval
- Basic file management (create, read, write)

**Acceptance Criteria**:
- Can store and retrieve account data to/from files
- Can store and retrieve book data to/from files
- Stays within 20 file limit
- No "filesystem as database" violations

---

### M3: Account System Implementation
**Status**: Planned  
**Estimated Cycles**: 10  
**Description**: Implement complete account system with all commands and privilege management.
- Login stack management
- Commands: su, logout, register, passwd, useradd, delete
- Privilege checking (0, 1, 3, 7)
- Password validation
- Root account auto-creation on first run

**Acceptance Criteria**:
- All account commands work correctly
- Privilege system enforces access control
- Login stack behaves as specified
- Root account created automatically
- Passes all account-related test cases

---

### M4: Book System Implementation
**Status**: Planned  
**Estimated Cycles**: 12  
**Description**: Implement complete book system with search, purchase, selection, modification, and import.
- Commands: show, buy, select, modify, import
- ISBN-based indexing
- Keyword, author, name search with multiple indexes
- Book selection per login session
- Inventory management

**Acceptance Criteria**:
- All book commands work correctly
- Search works efficiently with all parameters
- Book selection persists per login session
- Inventory updates correctly
- Passes all book-related test cases

---

### M5: Log System & Financial Tracking
**Status**: Planned  
**Estimated Cycles**: 6  
**Description**: Implement logging and financial tracking features.
- Commands: show finance, log, report finance, report employee
- Transaction recording (income/expenditure)
- Employee operation logging
- Query last N transactions

**Acceptance Criteria**:
- Financial tracking works correctly
- Log commands produce required output
- Reports are readable and correct
- Passes all log-related test cases

---

### M6: Integration Testing & Bug Fixing
**Status**: Planned  
**Estimated Cycles**: 10  
**Description**: Run all 107 test cases, identify and fix bugs, ensure edge cases are handled.
- Test against all data/004/1075/*.in files
- Fix bugs found during testing
- Optimize performance if needed
- Handle edge cases

**Acceptance Criteria**:
- Passes all 107 test cases in data/004/1075/
- No memory leaks
- Stays within time/memory limits (10s, 64MB)
- Output matches expected format exactly

---

### M7: Final Verification & Submission Preparation
**Status**: Planned  
**Estimated Cycles**: 4  
**Description**: Final verification, code cleanup, and preparation for ACMOJ submission.
- Final round of testing
- Code review and cleanup
- Documentation
- Verify git repository is ready for submission
- Mark project complete for external evaluation

**Acceptance Criteria**:
- All tests pass consistently
- Code is clean and well-organized
- Git repository is properly configured
- Ready for ACMOJ submission

---

## Total Estimated Cycles: 53

## Notes
- The system must use file-based persistence, not in-memory storage
- Maximum 20 files allowed
- Must compile with CMake + Make to produce `code` executable
- Performance is critical - may need B+ tree or similar for efficiency
- Edge cases and error handling are important for hidden tests (1775)

## Lessons Learned
*(Will be updated as we progress)*
