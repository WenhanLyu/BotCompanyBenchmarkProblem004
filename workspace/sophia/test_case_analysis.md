# Test Case Analysis: Cases 1-30 in data/004/1075/

## Overview
Analysis of test cases 1-30 reveals distinct test categories with varying complexity levels. The system is a bookstore management system with user account management (privilege levels) and book operations.

---

## Test Categorization

### **Category A: Account System Focus (Tests 1-3, 8)**

#### **Test 1: Basic Account Hierarchy**
- **Focus**: Multi-level user creation and login flow
- **Commands Used**:
  - `su <user> <password>` - Switch user
  - `useradd <username> <password> <privilege> <name>` - Create user (requires privilege 3/7)
  - `register <username> <password> <name>` - Self-registration (creates privilege 1 user)
  - `logout` - Log out current user
  - `exit` - Exit system
- **Key Scenarios**:
  - Root (privilege 7) creates workers (privilege 3)
  - Workers create users (privilege 1)
  - Users can register themselves
  - Login/logout cycles for verification
- **Complexity**: Low - straightforward hierarchy test
- **M2 Target**: ✅ **YES** - Essential for M2 account system

#### **Test 2: Advanced Account Operations**
- **Focus**: Password changes, user deletion, account lifecycle
- **Commands Used**:
  - All from Test 1, plus:
  - `passwd <username> <old_password> <new_password>` - Change password (requires privilege 7, or user changing own)
  - `delete <username>` - Delete user (requires privilege 7)
- **Key Scenarios**:
  - Root changes own password
  - Worker creates users, then gets deleted
  - Orphaned user (user0) tries to change another user's password
  - Password changes for registered users
  - User deletion and recreation
- **Complexity**: Medium - tests edge cases in account management
- **M2 Target**: ✅ **YES** - Critical for M2

#### **Test 3: Error Handling & Invalid Operations**
- **Focus**: Testing system robustness against invalid commands
- **Commands Used**: All account commands with invalid inputs
- **Key Scenarios**:
  - Invalid commands before login ("Always Challenge Miracles", "Always Cook Mushroom")
  - Duplicate user creation attempts
  - Creating users with reserved names (root, root2)
  - Wrong password attempts
  - Deleting non-existent users
  - Invalid command formats (missing parameters, spaces in names)
  - Privilege violations (users trying admin operations)
  - Changing passwords without proper privileges
- **Complexity**: High - comprehensive error testing
- **M2 Target**: ✅ **YES** - Essential for robust M2

#### **Test 8: Failed Login Attempt**
- **Focus**: Authentication failure
- **Commands**: `su root sjtu`, `su worker worker_password` (before worker exists)
- **Scenario**: Attempting to login as non-existent user
- **Complexity**: Low - basic error case
- **M2 Target**: ✅ **YES** - Basic auth validation

---

### **Category B: Integrated Account + Book Operations (Tests 4-7, 9)**

#### **Test 4: Worker Book Management**
- **Focus**: Privilege 3 (worker) book operations
- **Commands Used**:
  - Account commands (su, logout)
  - `select <ISBN>` - Select book for modification (privilege 3+)
  - `modify -ISBN=<> -name=<> -author=<> -keyword=<> -price=<>` - Modify book attributes
  - `import <quantity> <cost>` - Import books (privilege 3+)
  - `show [-ISBN=<> | -name=<> | -author=<> | -keyword=<>]` - Query books (all users)
- **Key Scenarios**:
  - Worker selects and modifies multiple books
  - User (privilege 1) can only view books
  - Multiple attribute modifications
- **Complexity**: Medium - tests privilege separation
- **M2 Target**: ⚠️ **PARTIAL** - Only account/auth parts relevant to M2

#### **Test 5: Complete Financial Flow**
- **Focus**: Multi-user buying, importing, financial tracking
- **Commands Used**:
  - All from Test 4, plus:
  - `buy <ISBN> <quantity>` - Purchase books (all users)
  - `show finance [count]` - Show financial records (privilege 7 only)
- **Key Scenarios**:
  - Root creates workers before operations
  - Worker imports books
  - Regular user buys books
  - Worker also buys books
  - Root buys and imports books
  - Financial report viewing at different time counts
- **Complexity**: High - full system integration
- **M2 Target**: ⚠️ **PARTIAL** - Auth checks matter to M2

#### **Test 6: Book Selection Edge Cases**
- **Focus**: Book modification without proper selection, ISBN changes
- **Commands**: modify, import, select, show, buy
- **Key Scenarios**:
  - Attempting modify/import without selecting a book first
  - Changing ISBN of selected book
  - Multiple ISBN changes
- **Complexity**: Medium - edge cases in book system
- **M2 Target**: ❌ **NO** - Book-focused, minimal account testing

#### **Test 7: Simple Worker Operations**
- **Focus**: Basic worker workflow
- **Commands**: su, logout, select, modify, exit
- **Scenario**: Worker logs in, modifies books, logs out
- **Complexity**: Low - basic flow
- **M2 Target**: ⚠️ **PARTIAL** - Minimal account coverage

#### **Test 9: Guest User Operations**
- **Focus**: Unauthenticated user capabilities
- **Commands**: su, logout, register, show
- **Scenario**: User registers and views books without logging in as admin
- **Complexity**: Low - basic registration flow
- **M2 Target**: ✅ **YES** - Tests registration without admin

---

### **Category C: Book Data Queries (Tests 10-30)**

#### **Test 10: Massive Book Creation (Stress Test)**
- **Focus**: Large-scale book data setup
- **Commands**: su, logout, useradd, select, modify (2000+ operations)
- **Key Scenario**: Worker creates 1000 books with sequential data
- **Complexity**: Very High - performance/stress test
- **Data Pattern**: Sequential ISBNs (1234567890 to 123456789999), names like "0's_book" to "999's_book"
- **M2 Target**: ❌ **NO** - Pure data setup, not account-focused

#### **Tests 11-30: Book Query Tests**
- **Focus**: Query validation after Test 10 setup
- **Pattern**: Each test does:
  1. `su root sjtu`
  2. `logout`
  3. `su worker worker_password`
  4. Single `show` query with different parameters
  5. `exit`
- **Query Types**:
  - ISBN queries: Tests 12, 15, 18, 21, 24, 27, 30
  - Author queries: Tests 13, 16, 19, 22, 25, 28
  - Keyword queries: Tests 11, 14, 17, 20, 23, 26, 29
- **Complexity**: Low - simple queries
- **M2 Target**: ❌ **NO** - Tests book query system, not accounts

---

## Commands Summary by Privilege Level

### Privilege 7 (Root):
- useradd
- passwd (any user)
- delete
- show finance
- All privilege 3 and 1 commands

### Privilege 3 (Worker):
- select
- modify
- import
- All privilege 1 commands

### Privilege 1 (User):
- buy
- show (books)
- passwd (own password only)

### No login required:
- register
- su
- logout
- exit

---

## M2 Recommendations

### **Priority 1: Essential for M2**
1. **Test 1** - Basic account hierarchy and login flow
2. **Test 2** - Password changes and user deletion
3. **Test 3** - Comprehensive error handling
4. **Test 8** - Authentication failure

### **Priority 2: Useful for M2**
5. **Test 9** - Registration without admin
6. **Test 4 (partial)** - Privilege-based command access validation
7. **Test 5 (partial)** - Multi-user privilege verification

### **Not Recommended for M2**
- Tests 6-7: Too book-focused
- Tests 10-30: Pure book query tests, no significant account logic

---

## Key Account System Features to Implement

Based on test analysis, M2 should focus on:

1. **User Management**:
   - Create users with different privilege levels (1, 3, 7)
   - Default root account (root/sjtu, privilege 7)
   - Track currently logged-in user

2. **Authentication**:
   - Login (su command)
   - Logout
   - Password validation
   - Session management

3. **Authorization**:
   - Privilege level checks before command execution
   - Commands require specific privilege levels
   - Users can only change their own passwords (except root)

4. **User Operations**:
   - useradd (privilege 7 only)
   - register (anyone, creates privilege 1)
   - passwd (user for self, privilege 7 for others)
   - delete (privilege 7 only)

5. **Error Handling**:
   - Invalid login attempts
   - Duplicate usernames
   - Insufficient privileges
   - Invalid command formats
   - Operations on non-existent users

6. **Edge Cases**:
   - User deletion cascade (what happens to users created by deleted user)
   - Password changes during active sessions
   - Multiple login attempts
   - Reserved usernames

---

## Test Execution Strategy for M2

### Phase 1: Core Account System
Run Tests 1, 2, 3, 8 to validate basic account functionality

### Phase 2: Extended Features
Run Test 9 and partial validation from Tests 4-5

### Phase 3: Stress & Edge Cases
Re-run Test 3 with additional edge cases if needed

**Total M2-relevant tests: 4-6 tests** (out of 30 examined)
