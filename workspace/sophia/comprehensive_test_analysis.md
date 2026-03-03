# Comprehensive Test Data Analysis
## Bookstore Management System - Test Suite Analysis

### Executive Summary
Analyzed all 214 test files in data/004/1075/ directory.

### Command Distribution Analysis

#### Tests Using 'show finance' Command
**Count: 28 tests**

**Test Numbers:**
- Simple tests: 5, 6
- Complex stress tests: 180, 181, 182, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 201, 202, 204, 205, 206, 208, 209

**Pattern Observations:**
1. Tests 5-6 are early/simple tests
2. Tests 180-209 are late-stage complex/stress tests
3. The 'show finance' command appears 354 times total across these 28 tests
4. 'show finance' is often called with time parameters: `show finance`, `show finance 1`, `show finance 3`, `show finance 5`, `show finance 10`

#### Other Log/Report Commands
**Count: 0 tests**

- `report finance`: NOT FOUND in any test
- `report employee`: NOT FOUND in any test  
- `log` command (standalone): NOT FOUND in any test

**Conclusion:** Only the 'show finance' command is tested. No other finance or employee reporting/logging commands exist in the test suite.

---

### Test File Size Distribution

#### Largest Tests (Stress/Performance Tests)
- 179.in: 7,620 lines
- 173.in: 7,613 lines
- 177.in: 7,611 lines
- 176.in: 7,607 lines
- 170.in: 7,606 lines

Tests 170-179 appear to be major stress tests (7,500-7,600 lines each)
Tests 190-199 are also large (7,400-7,500 lines each)

#### Smallest Tests
Tests 1-50 are typically small (50-200 lines)

---

### Primary Command Categories

Based on frequency analysis across all tests:

1. **Book Operations (Core)**
   - `buy`: 107,195 occurrences (dominant command)
   - `show`: 56,161 occurrences (book queries)
   - `select`: 14,249 occurrences
   - `modify`: 13,438 occurrences
   - `import`: 6,181 occurrences
   - `delete`: 137 occurrences

2. **User Management**
   - `su`: 49,668 occurrences (user switching)
   - `logout`: 49,313 occurrences
   - `passwd`: 20,382 occurrences
   - `register`: 3,145 occurrences
   - `useradd`: 500 occurrences

3. **Finance Reporting**
   - `show finance`: 354 occurrences (28 tests only)

4. **System**
   - `exit`: 113 occurrences

---

### Test Categories

#### 1. Simple Functionality Tests (Tests 1-50)
- Basic CRUD operations
- Simple user authentication
- Single-user workflows
- Small data volumes

#### 2. Medium Complexity Tests (Tests 51-100)
- Multi-user scenarios
- Moderate data volumes
- Basic privilege testing

#### 3. Advanced Tests (Tests 101-169)
- Complex multi-user interactions
- Edge cases
- Data integrity tests

#### 4. Large Stress Tests (Tests 170-179)
- ~7,500-7,600 lines each
- High volume transactions
- Performance testing
- Does NOT include 'show finance'

#### 5. Finance Reporting Tests (Tests 180-209, subset)
- 28 tests specifically test 'show finance'
- Mix of sizes (medium to large)
- ~7,400-7,500 lines for larger ones
- Test both basic and time-windowed finance reporting

---

### 'show finance' Command Pattern Analysis

From examining test 5:
```
show finance          # Show all transactions
show finance 1        # Show transactions from timestamp 1
show finance 3        # Show transactions from timestamp 3  
show finance 5        # Show transactions from timestamp 5
show finance 10       # Show transactions from timestamp 10
```

The command supports:
- No parameter: all-time finance report
- Single parameter: finance report from specific timestamp

---

### Book Query Patterns

The 'show' command (for books) is heavily used with filters:
- By ISBN: `-ISBN=978-X-XXXX-XXXX-X`
- By keyword: `-keyword="term"`
- By author: `-author="name"`
- By name: `-name="title"`

Most frequent queries:
- show -ISBN=978-0-366-07286-6 (624 times)
- show -ISBN=978-0-9985935-1-7 (554 times)
- show -keyword="cylinder" (406 times)
- show -author="BobbyBrooks" (384 times)

---

### Test Coverage Insights

**Well-Covered Areas:**
1. Book purchasing (buy command - 107K occurrences)
2. Book inventory management (show, select, modify, import)
3. User authentication and session management
4. Multi-user concurrent operations (stress tests)

**Finance Reporting Coverage:**
- Only 28 out of 214 tests (13%) test finance functionality
- These are clustered: tests 5-6 (early simple) and 180-209 (late complex)
- Suggests finance reporting was either:
  - Added later in development
  - Considered advanced/optional feature
  - Requires specific setup/state

**Not Tested (Not Found):**
- Employee reporting
- Other log commands
- Any 'report finance' or 'report employee' commands

---

### Recommendations for Development

1. **Priority Order:**
   - Implement core book operations first (buy, show, select, modify, import)
   - Implement user management (su, logout, register, useradd, passwd)
   - Implement finance reporting LAST (only 13% of tests)

2. **Testing Strategy:**
   - Start with tests 1-50 (simple cases)
   - Progress to 51-169 (medium complexity)
   - Tackle stress tests 170-179 (no finance)
   - Finish with finance tests 180-209 subset

3. **Finance Implementation:**
   - Required for only 28 tests
   - Must support time-windowed queries
   - Format: `show finance [timestamp]`
   - Critical for passing ~13% of test suite

4. **Performance Considerations:**
   - Largest tests have ~7,600 lines
   - Must handle 100K+ buy operations efficiently
   - 50K+ user switches (su/logout cycles)

---

### Test Number Distribution by Category

**Tests WITHOUT 'show finance':** 186 tests
(1-4, 7-179, 183, 200, 203, 207, 210-214)

**Tests WITH 'show finance':** 28 tests  
(5, 6, 180, 181, 182, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 201, 202, 204, 205, 206, 208, 209)

---

### Key Findings

1. **No employee or alternative log commands exist** - only 'show finance'
2. **Finance testing is concentrated** in specific test numbers (5-6, 180-209)
3. **Majority of tests focus on core bookstore operations** (87%)
4. **Stress tests are massive** (7,500+ lines) but don't test finance
5. **'buy' is the most frequent operation** by far (107K occurrences)
