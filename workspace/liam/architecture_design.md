# File-Based Storage Architecture Design for Bookstore Management System

**Author**: Liam (C++ Architecture Specialist)  
**Date**: 2024  
**Version**: 1.0

---

## Executive Summary

This document proposes a comprehensive architecture for implementing file-based persistent storage for the Bookstore Management System. The design focuses on efficiency, scalability, and adherence to the 20-file constraint while supporting tens of thousands of accounts and books with hundreds of thousands of operations.

---

## 1. Architecture Overview

### 1.1 Design Philosophy

The architecture uses a **hybrid approach**:
- **Block-based file storage** for raw data persistence
- **In-memory B+ tree indexes** for fast lookups (rebuilt on startup)
- **Sequential log files** for transaction history
- **Separate index files** for multi-attribute searching

This approach balances:
- **Performance**: O(log n) lookups via B+ trees
- **Memory efficiency**: Load only necessary blocks, not entire datasets
- **Persistence**: All data stored in files
- **File limit**: Uses only 12-15 files (well under 20)

### 1.2 Key Components

```
┌─────────────────────────────────────────────────────────────┐
│                    Application Layer                         │
│  (Account System, Book System, Log System, Command Parser)   │
└─────────────────────────────────────────────────────────────┘
                            │
┌─────────────────────────────────────────────────────────────┐
│                  Storage Engine Layer                         │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │   Account    │  │     Book     │  │  Transaction │      │
│  │   Manager    │  │   Manager    │  │    Logger    │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
└─────────────────────────────────────────────────────────────┘
                            │
┌─────────────────────────────────────────────────────────────┐
│                  Index & Cache Layer                          │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │  B+ Tree     │  │  B+ Tree     │  │   LRU Cache  │      │
│  │  (UserID)    │  │  (ISBN)      │  │   (Blocks)   │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │  B+ Tree     │  │  B+ Tree     │  │  B+ Tree     │      │
│  │  (Author)    │  │  (Name)      │  │  (Keyword)   │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
└─────────────────────────────────────────────────────────────┘
                            │
┌─────────────────────────────────────────────────────────────┐
│                     File Layer (15 files)                     │
│                                                               │
│  Data Files (8):                                              │
│    - accounts.dat       (account records)                     │
│    - books.dat          (book records)                        │
│    - account_idx.dat    (UserID -> offset mapping)            │
│    - isbn_idx.dat       (ISBN -> offset mapping)              │
│    - author_idx.dat     (Author -> ISBN list)                 │
│    - name_idx.dat       (Name -> ISBN list)                   │
│    - keyword_idx.dat    (Keyword -> ISBN list)                │
│    - metadata.dat       (system metadata, counters)           │
│                                                               │
│  Log Files (3):                                               │
│    - transactions.log   (financial transactions)              │
│    - operations.log     (employee operations)                 │
│    - system.log         (system events)                       │
│                                                               │
│  Configuration (1):                                           │
│    - config.dat         (initialization flag, root account)   │
│                                                               │
│  Total: 12 files (8 files buffer remaining)                   │
└─────────────────────────────────────────────────────────────┘
```

---

## 2. File Organization Strategy

### 2.1 File Allocation (12 files total)

| File Name          | Purpose                                    | Size Estimate | Access Pattern      |
|--------------------|--------------------------------------------|---------------|---------------------|
| `accounts.dat`     | Account data records (sequential)          | ~10 MB        | Random read/append  |
| `books.dat`        | Book data records (sequential)             | ~10 MB        | Random read/write   |
| `account_idx.dat`  | UserID → offset index                      | ~2 MB         | Random read/write   |
| `isbn_idx.dat`     | ISBN → offset index                        | ~2 MB         | Random read/write   |
| `author_idx.dat`   | Author → ISBN list index                   | ~3 MB         | Random read/write   |
| `name_idx.dat`     | Name → ISBN list index                     | ~3 MB         | Random read/write   |
| `keyword_idx.dat`  | Keyword → ISBN list index                  | ~5 MB         | Random read/write   |
| `metadata.dat`     | Counters, free lists, system state         | ~100 KB       | Sequential read/write |
| `transactions.log` | Financial transaction history              | ~5 MB         | Append-only         |
| `operations.log`   | Employee operations log                    | ~5 MB         | Append-only         |
| `system.log`       | System events (login, logout, etc.)        | ~5 MB         | Append-only         |
| `config.dat`       | Initialization flag, system config         | ~1 KB         | Read once on start  |

**Total**: 12 files, ~50 MB total storage (well within 1024 MB limit)

### 2.2 Design Rationale

#### Why This Structure?

1. **Separation of Data and Indexes**:
   - Data files (`accounts.dat`, `books.dat`) store raw records
   - Index files store mappings for fast lookup
   - Allows independent optimization of each component

2. **Multiple Index Files**:
   - Each search criterion (ISBN, author, name, keyword) has its own index
   - Enables O(log n) lookups for all search types
   - Avoids full table scans

3. **Append-Only Logs**:
   - Minimal overhead for logging
   - No random writes during transaction processing
   - Easy to query last N transactions

4. **Metadata Separation**:
   - Centralized system state management
   - Atomic updates to counters and flags
   - Simplifies initialization logic

---

## 3. Data Structure Design

### 3.1 Account Storage

#### File: `accounts.dat`

**Structure**: Fixed-size record file with free list management

```cpp
// Fixed-size record: 128 bytes
struct AccountRecord {
    char user_id[32];        // null-terminated
    char password[32];       // null-terminated (or hashed)
    char username[32];       // null-terminated
    uint8_t privilege;       // 0, 1, 3, or 7
    bool is_deleted;         // soft delete flag
    uint8_t padding[30];     // reserved for future use
};
```

**Layout**:
```
[Header: 64 bytes]
  - Total records: 8 bytes
  - First free offset: 8 bytes
  - Reserved: 48 bytes

[Record 0: 128 bytes]
[Record 1: 128 bytes]
...
[Record N: 128 bytes]
```

**Operations**:
- **Insert**: Append to end or reuse free slot
- **Read**: Seek to offset, read 128 bytes
- **Update**: Seek to offset, overwrite 128 bytes
- **Delete**: Mark `is_deleted = true`, add to free list

**Time Complexity**:
- Insert: O(1) file I/O
- Read: O(1) file I/O (given offset)
- Update: O(1) file I/O
- Delete: O(1) file I/O

#### File: `account_idx.dat`

**Structure**: Serialized B+ tree index (UserID → offset)

```cpp
// In-memory B+ tree node (not stored directly, rebuilt on load)
struct BPlusNode {
    bool is_leaf;
    int num_keys;
    string keys[ORDER];
    union {
        long offsets[ORDER];      // for leaf nodes
        BPlusNode* children[ORDER+1]; // for internal nodes
    };
};

// On-disk format (serialized)
struct IndexEntry {
    char user_id[32];
    long offset;              // offset in accounts.dat
};
```

**Serialization Format**:
```
[Header: 32 bytes]
  - Entry count: 8 bytes
  - Reserved: 24 bytes

[Entry 0: 40 bytes]
  - user_id: 32 bytes
  - offset: 8 bytes

[Entry 1: 40 bytes]
...
```

**Load Strategy**:
1. On startup, read all entries from `account_idx.dat`
2. Build in-memory B+ tree from sorted entries
3. Keep B+ tree in memory for fast lookups (< 1 MB for 10,000 accounts)

**Save Strategy**:
1. On shutdown or periodic checkpoint, traverse B+ tree
2. Write all leaf entries to `account_idx.dat` in sorted order
3. Atomic file replacement (write to `.tmp`, rename)

**Time Complexity**:
- Lookup: O(log n) in-memory
- Insert: O(log n) in-memory, O(1) amortized disk write
- Delete: O(log n) in-memory, O(1) amortized disk write

### 3.2 Book Storage

#### File: `books.dat`

**Structure**: Variable-size record file with offset tracking

```cpp
// Variable-size record
struct BookRecord {
    uint16_t record_size;     // total size of this record
    char isbn[24];            // ISBN (20 + padding)
    char name[64];            // Book name
    char author[64];          // Author name
    char keyword[64];         // Keywords (pipe-separated)
    double price;             // Unit price
    int stock_quantity;       // Current inventory
    bool is_deleted;          // Soft delete flag
    uint8_t padding[2];       // alignment
};
// Total: ~210 bytes per record
```

**Layout**:
```
[Header: 64 bytes]
  - Total records: 8 bytes
  - Total size: 8 bytes
  - Reserved: 48 bytes

[Record 0]
  - record_size: 2 bytes
  - data: variable

[Record 1]
  - record_size: 2 bytes
  - data: variable
...
```

**Operations**:
- **Insert**: Append to end
- **Read**: Seek to offset, read `record_size`, read data
- **Update**: If size unchanged, overwrite in place; else mark deleted + append new
- **Delete**: Mark `is_deleted = true`

**Garbage Collection**:
- Periodically (e.g., every 10,000 operations), compact the file
- Copy non-deleted records to new file
- Rebuild all indexes

#### File: `isbn_idx.dat`

**Structure**: Similar to `account_idx.dat`, maps ISBN → offset

```cpp
struct ISBNIndexEntry {
    char isbn[24];
    long offset;              // offset in books.dat
};
```

**In-Memory**: B+ tree (ISBN → offset), < 500 KB for 10,000 books

#### Files: `author_idx.dat`, `name_idx.dat`, `keyword_idx.dat`

**Structure**: Multi-value index (attribute → list of ISBNs)

```cpp
// On-disk format
struct MultiIndexEntry {
    char key[64];             // author/name/keyword
    uint16_t isbn_count;      // number of ISBNs with this key
    char isbns[][24];         // variable-length array of ISBNs
};
```

**In-Memory**: `std::map<string, std::vector<string>>`
- Author → list of ISBNs
- Name → list of ISBNs  
- Keyword → list of ISBNs

**Example**:
```
author_idx: "J.K. Rowling" → ["ISBN001", "ISBN002", "ISBN003"]
keyword_idx: "magic" → ["ISBN001", "ISBN004", "ISBN007"]
```

**Load Strategy**:
1. On startup, read all entries from `author_idx.dat`, `name_idx.dat`, `keyword_idx.dat`
2. Build in-memory maps
3. Memory usage: ~3-5 MB for 10,000 books (tolerable within 64 MB limit)

**Update Strategy**:
- When book added/modified: update in-memory map + mark index dirty
- On shutdown or checkpoint: write entire map to file

**Search Strategy**:
1. Look up key in map → get list of ISBNs
2. For each ISBN, look up in `isbn_idx.dat` → get offset
3. Read book record from `books.dat` at offset
4. Sort results by ISBN (in memory)

**Time Complexity**:
- Lookup: O(log n) for map lookup + O(k) to fetch k results
- Insert: O(log n) in-memory
- Search with keyword: O(k log k) where k = result count

### 3.3 Log Storage

#### File: `transactions.log`

**Structure**: Append-only log with fixed-size entries

```cpp
struct TransactionEntry {
    uint64_t timestamp;       // Unix timestamp
    double amount;            // positive for income, negative for expenditure
    char type[16];            // "BUY" or "IMPORT"
    char isbn[24];            // related book (if applicable)
    int quantity;             // quantity involved
    char user_id[32];         // who performed the operation
    uint8_t padding[16];      // reserved
};
// Total: 128 bytes per entry
```

**Layout**:
```
[Header: 64 bytes]
  - Entry count: 8 bytes
  - Total income: 8 bytes
  - Total expenditure: 8 bytes
  - Reserved: 40 bytes

[Entry 0: 128 bytes]
[Entry 1: 128 bytes]
...
```

**Operations**:
- **Append**: Write entry to end, update header counters
- **Query last N**: Seek to (total_count - N) * 128 + 64, read N entries
- **Query all**: Read entire file (acceptable for tens of thousands of entries)

**Time Complexity**:
- Append: O(1) file I/O
- Query last N: O(N) file I/O
- Query all: O(total) file I/O

#### File: `operations.log`

**Structure**: Append-only log with variable-size entries

```cpp
struct OperationEntry {
    uint64_t timestamp;       // Unix timestamp
    char user_id[32];         // who performed the operation
    uint16_t command_length;  // length of command string
    char command[];           // variable-length command string
};
```

**Operations**:
- **Append**: Write entry to end
- **Read all**: Sequential read of entire file

#### File: `system.log`

**Structure**: Similar to `operations.log`, logs system events

```cpp
struct SystemLogEntry {
    uint64_t timestamp;
    char event_type[16];      // "LOGIN", "LOGOUT", "STARTUP", etc.
    char user_id[32];
    char details[128];        // additional details
};
```

### 3.4 Metadata Storage

#### File: `metadata.dat`

**Structure**: Key-value store for system metadata

```cpp
struct Metadata {
    bool initialized;          // has the system been initialized?
    uint64_t account_count;    // total accounts
    uint64_t book_count;       // total books
    uint64_t transaction_count; // total transactions
    uint64_t operation_count;  // total operations
    long account_free_list;    // first free slot in accounts.dat
    long book_free_list;       // first free slot in books.dat
    uint8_t reserved[128];     // for future use
};
```

**Operations**:
- **Read**: Load entire file on startup
- **Write**: Overwrite entire file on shutdown or checkpoint

#### File: `config.dat`

**Structure**: System configuration

```cpp
struct Config {
    bool first_run;           // is this the first run?
    char root_password[32];   // root account password (default: "sjtu")
    uint8_t reserved[64];     // for future use
};
```

**Operations**:
- **Read**: Load on startup
- **Write**: Update after first run

---

## 4. Indexing Strategy

### 4.1 Primary Indexes (Unique Keys)

#### Account Primary Index (UserID)

**Structure**: B+ tree (in-memory) + serialized index file

**Properties**:
- **Key**: UserID (string, max 30 chars)
- **Value**: Offset in `accounts.dat` (long)
- **Uniqueness**: Enforced (no duplicate UserIDs)

**B+ Tree Configuration**:
- **Order**: 100 (each node has 50-100 keys)
- **Height**: log₁₀₀(10,000) ≈ 3 levels for 10,000 accounts
- **Memory**: ~1 MB for 10,000 accounts

**Operations**:
```cpp
// Insert account
1. Check if UserID exists in B+ tree → O(log n)
2. If exists, return error (duplicate)
3. Append account record to accounts.dat → O(1)
4. Insert (UserID, offset) into B+ tree → O(log n)

// Lookup account
1. Search UserID in B+ tree → O(log n)
2. Get offset
3. Seek to offset in accounts.dat, read record → O(1)

// Delete account
1. Search UserID in B+ tree → O(log n)
2. Get offset
3. Mark record as deleted in accounts.dat → O(1)
4. Remove from B+ tree → O(log n)
```

#### Book Primary Index (ISBN)

**Structure**: B+ tree (in-memory) + serialized index file

**Properties**:
- **Key**: ISBN (string, max 20 chars)
- **Value**: Offset in `books.dat` (long)
- **Uniqueness**: Enforced (no duplicate ISBNs)

**B+ Tree Configuration**:
- **Order**: 100
- **Height**: log₁₀₀(10,000) ≈ 3 levels for 10,000 books
- **Memory**: ~500 KB for 10,000 books

**Operations**: Similar to Account Primary Index

### 4.2 Secondary Indexes (Multi-Value Keys)

#### Author Index

**Structure**: `std::map<string, std::vector<string>>` (in-memory) + serialized file

**Properties**:
- **Key**: Author name (string, max 60 chars)
- **Value**: List of ISBNs (vector of strings)
- **Uniqueness**: One author can have multiple books

**Example**:
```
"J.K. Rowling" → ["9780439708180", "9780439064873"]
"George Orwell" → ["9780451524935"]
```

**Operations**:
```cpp
// Search by author
1. Look up author in map → O(log n)
2. Get list of ISBNs → O(1)
3. For each ISBN, look up in ISBN index → O(k log n) where k = result count
4. Read book records → O(k)
5. Sort by ISBN → O(k log k)
6. Return results

// Add book
1. Insert into ISBN index (primary) → O(log n)
2. Extract author from book record
3. Add ISBN to author's list in author index → O(log n)

// Modify book author
1. Remove ISBN from old author's list → O(log n)
2. Add ISBN to new author's list → O(log n)
3. Update book record → O(1)
```

**Memory**: ~1-2 MB for 10,000 books (assuming average 2 books per author)

#### Name Index

**Structure**: Same as Author Index

**Properties**:
- **Key**: Book name (string, max 60 chars)
- **Value**: List of ISBNs
- **Uniqueness**: Multiple books can have the same name (different editions)

#### Keyword Index

**Structure**: Same as Author Index, but with special handling

**Properties**:
- **Key**: Single keyword (string, max 60 chars)
- **Value**: List of ISBNs
- **Note**: Each book can have multiple keywords (pipe-separated)

**Example**:
```
"magic" → ["9780439708180", "9780439064873", "9781408855652"]
"fantasy" → ["9780439708180", "9780261102385"]
"dystopia" → ["9780451524935", "9780547928227"]
```

**Special Handling**:
```cpp
// Add book with keywords "magic|fantasy|adventure"
1. Split keywords by '|' → ["magic", "fantasy", "adventure"]
2. For each keyword:
   - Add ISBN to keyword's list in keyword index → O(k log n) where k = keyword count

// Search by keyword
1. Look up keyword in map (exact match, no substring) → O(log n)
2. Get list of ISBNs → O(1)
3. Fetch and sort results → O(k log k)
```

### 4.3 Index Persistence Strategy

#### Load on Startup (Cold Start)

```cpp
void loadIndexes() {
    // 1. Load account index
    loadAccountIndex("account_idx.dat");  // ~1 MB, O(n) time
    
    // 2. Load book indexes
    loadISBNIndex("isbn_idx.dat");        // ~500 KB, O(n) time
    loadAuthorIndex("author_idx.dat");    // ~1 MB, O(n) time
    loadNameIndex("name_idx.dat");        // ~1 MB, O(n) time
    loadKeywordIndex("keyword_idx.dat");  // ~2 MB, O(n) time
    
    // Total: ~5.5 MB loaded, ~0.1 seconds for 10,000 records
}
```

#### Save on Shutdown (Checkpoint)

```cpp
void saveIndexes() {
    // 1. Traverse B+ tree, write entries to index file
    saveAccountIndex("account_idx.dat");
    saveISBNIndex("isbn_idx.dat");
    
    // 2. Iterate maps, write entries to index file
    saveAuthorIndex("author_idx.dat");
    saveNameIndex("name_idx.dat");
    saveKeywordIndex("keyword_idx.dat");
    
    // 3. Update metadata
    saveMetadata("metadata.dat");
}
```

#### Periodic Checkpointing (Optional)

- Every 10,000 operations, save indexes to disk
- Reduces recovery time in case of crash
- Trade-off: Slight performance overhead vs. data safety

### 4.4 Index Consistency

**Invariant**: All indexes must be consistent with data files

**Strategy**:
1. **Atomic Updates**: Update in-memory index only after data file write succeeds
2. **Rollback on Failure**: If data write fails, don't update index
3. **Rebuild on Corruption**: If index file is corrupted, rebuild from data file (slow but safe)

**Rebuild Algorithm** (Emergency Recovery):
```cpp
void rebuildIndexes() {
    // 1. Clear all in-memory indexes
    clearIndexes();
    
    // 2. Scan accounts.dat, rebuild account index
    for (each account in accounts.dat) {
        if (!account.is_deleted) {
            accountIndex.insert(account.user_id, offset);
        }
    }
    
    // 3. Scan books.dat, rebuild book indexes
    for (each book in books.dat) {
        if (!book.is_deleted) {
            isbnIndex.insert(book.isbn, offset);
            authorIndex.add(book.author, book.isbn);
            nameIndex.add(book.name, book.isbn);
            for (each keyword in book.keyword.split('|')) {
                keywordIndex.add(keyword, book.isbn);
            }
        }
    }
    
    // 4. Save rebuilt indexes
    saveIndexes();
}
```

**Time Complexity**: O(n) for n records, acceptable for recovery scenario

---

## 5. Performance Analysis

### 5.1 Time Complexity

| Operation                  | Time Complexity          | Notes                                        |
|----------------------------|--------------------------|----------------------------------------------|
| Account lookup (UserID)    | O(log n)                 | B+ tree search                               |
| Account insert             | O(log n) + O(1) I/O      | B+ tree insert + file append                 |
| Account delete             | O(log n) + O(1) I/O      | B+ tree delete + file update                 |
| Book lookup (ISBN)         | O(log n)                 | B+ tree search                               |
| Book insert                | O(log n) + O(k log n)    | k = keyword count                            |
| Book search (author/name)  | O(log n) + O(k log k)    | k = result count, sorting                    |
| Book search (keyword)      | O(log n) + O(k log k)    | k = result count, sorting                    |
| Book search (all)          | O(n log n)               | Scan all books, sort by ISBN                 |
| Transaction log append     | O(1) I/O                 | Append to log file                           |
| Show finance (last N)      | O(N) I/O                 | Read last N entries from log                 |
| Show finance (all)         | O(total) I/O             | Read entire log file                         |

### 5.2 Space Complexity

| Component                  | Memory Usage             | Disk Usage              |
|----------------------------|--------------------------|-------------------------|
| Account B+ tree            | ~1 MB (10K accounts)     | ~400 KB (serialized)    |
| ISBN B+ tree               | ~500 KB (10K books)      | ~240 KB (serialized)    |
| Author index (map)         | ~1-2 MB (10K books)      | ~1-2 MB (serialized)    |
| Name index (map)           | ~1-2 MB (10K books)      | ~1-2 MB (serialized)    |
| Keyword index (map)        | ~2-3 MB (10K books)      | ~2-3 MB (serialized)    |
| Account records            | 0 (not in memory)        | ~1.3 MB (10K × 128B)    |
| Book records               | 0 (not in memory)        | ~2.1 MB (10K × 210B)    |
| Transaction log            | 0 (not in memory)        | ~12.8 MB (100K × 128B)  |
| **Total**                  | **~8-10 MB**             | **~25 MB**              |

**Analysis**:
- **Memory**: Well under 64 MB limit (leaves ~50 MB for program code, stack, etc.)
- **Disk**: Well under 1024 MB limit (only ~2.5% used)
- **Scalability**: Can handle 10× larger datasets (100K accounts/books) with ~80 MB memory

### 5.3 Performance Benchmarks (Estimated)

#### Scenario 1: Cold Start (Load Indexes)

```
Dataset: 10,000 accounts, 10,000 books
Operations:
  - Load account index: ~0.01 seconds
  - Load book indexes: ~0.04 seconds
  - Total: ~0.05 seconds
```

#### Scenario 2: Account Operations

```
Operations per second (estimated):
  - su (login): ~50,000 ops/sec (B+ tree lookup)
  - register: ~20,000 ops/sec (B+ tree insert + file I/O)
  - useradd: ~20,000 ops/sec
  - delete: ~20,000 ops/sec
```

#### Scenario 3: Book Operations

```
Operations per second (estimated):
  - show (ISBN): ~30,000 ops/sec (B+ tree lookup)
  - show (author/name): ~10,000 ops/sec (map lookup + fetch)
  - show (keyword): ~10,000 ops/sec
  - buy: ~15,000 ops/sec (lookup + update + log append)
  - import: ~15,000 ops/sec
  - modify: ~10,000 ops/sec (multiple index updates)
```

#### Scenario 4: Large Batch Test

```
Dataset: 10,000 accounts, 10,000 books
Operations: 100,000 mixed operations
Expected time:
  - Index load: 0.05 seconds
  - Operations: ~5 seconds
  - Index save: ~0.1 seconds
  - Total: ~5.2 seconds (well under 10 second limit)
```

### 5.4 Bottleneck Analysis

| Bottleneck               | Impact       | Mitigation                                     |
|--------------------------|--------------|------------------------------------------------|
| Disk I/O for data files  | Medium       | Block caching, batch writes                    |
| Index rebuild on startup | Low          | Indexes are small (~5 MB), loads in 0.05s      |
| Sorting search results   | Low          | Results are usually small (< 1000 items)       |
| Log file growth          | Low          | Append-only, no random access penalty          |
| Memory for indexes       | Very Low     | Only ~8-10 MB for 10K records                  |

**Critical Path**: Disk I/O for reading/writing book/account records

**Optimization**: LRU cache for frequently accessed records (optional, if needed)

---

## 6. Implementation Recommendations

### 6.1 Core Classes

```cpp
// File: storage_engine.h

class StorageEngine {
public:
    void initialize();
    void shutdown();
    
private:
    AccountManager accountMgr;
    BookManager bookMgr;
    LogManager logMgr;
    MetadataManager metaMgr;
};

class AccountManager {
public:
    bool insertAccount(const Account& acc);
    Account* getAccount(const string& userId);
    bool updateAccount(const Account& acc);
    bool deleteAccount(const string& userId);
    
private:
    BPlusTree<string, long> userIdIndex;  // in-memory
    FileManager accountFile;               // accounts.dat
    FileManager indexFile;                 // account_idx.dat
};

class BookManager {
public:
    bool insertBook(const Book& book);
    Book* getBook(const string& isbn);
    vector<Book> searchBooks(const SearchCriteria& criteria);
    bool updateBook(const Book& book);
    
private:
    BPlusTree<string, long> isbnIndex;           // in-memory
    map<string, vector<string>> authorIndex;      // in-memory
    map<string, vector<string>> nameIndex;        // in-memory
    map<string, vector<string>> keywordIndex;     // in-memory
    
    FileManager bookFile;       // books.dat
    FileManager isbnIdxFile;    // isbn_idx.dat
    FileManager authorIdxFile;  // author_idx.dat
    FileManager nameIdxFile;    // name_idx.dat
    FileManager keywordIdxFile; // keyword_idx.dat
};

class LogManager {
public:
    void logTransaction(const Transaction& txn);
    void logOperation(const Operation& op);
    vector<Transaction> getLastNTransactions(int n);
    
private:
    FileManager transactionLog;  // transactions.log
    FileManager operationLog;    // operations.log
    FileManager systemLog;       // system.log
};

class BPlusTree<Key, Value> {
public:
    bool insert(const Key& key, const Value& value);
    Value* search(const Key& key);
    bool remove(const Key& key);
    vector<pair<Key, Value>> getAll();  // for serialization
    
private:
    Node* root;
    int order;
};

class FileManager {
public:
    FileManager(const string& filename);
    void open();
    void close();
    long append(const void* data, size_t size);
    void read(long offset, void* data, size_t size);
    void write(long offset, const void* data, size_t size);
    
private:
    string filename;
    fstream file;
};
```

### 6.2 Initialization Sequence

```cpp
void StorageEngine::initialize() {
    // 1. Check if first run
    Config config = loadConfig("config.dat");
    
    if (config.first_run) {
        // First run: initialize system
        createAllFiles();
        createRootAccount();
        config.first_run = false;
        saveConfig(config);
    }
    
    // 2. Load metadata
    Metadata meta = loadMetadata("metadata.dat");
    
    // 3. Load all indexes into memory
    accountMgr.loadIndexes();
    bookMgr.loadIndexes();
    
    // Total time: ~0.05-0.1 seconds for 10K records
}
```

### 6.3 Shutdown Sequence

```cpp
void StorageEngine::shutdown() {
    // 1. Save all in-memory indexes to files
    accountMgr.saveIndexes();
    bookMgr.saveIndexes();
    
    // 2. Update metadata
    metaMgr.saveMetadata();
    
    // 3. Close all file handles
    accountMgr.closeFiles();
    bookMgr.closeFiles();
    logMgr.closeFiles();
}
```

### 6.4 Error Handling

```cpp
// Strategy: Fail-fast with error messages

bool AccountManager::insertAccount(const Account& acc) {
    try {
        // 1. Check if account exists
        if (userIdIndex.search(acc.userId) != nullptr) {
            return false;  // duplicate
        }
        
        // 2. Append to file
        long offset = accountFile.append(&acc, sizeof(Account));
        
        // 3. Update index (only if file write succeeded)
        userIdIndex.insert(acc.userId, offset);
        
        return true;
    } catch (const IOException& e) {
        // Log error, rollback, return false
        cerr << "Error inserting account: " << e.what() << endl;
        return false;
    }
}
```

### 6.5 Testing Strategy

```cpp
// Unit tests for each component

TEST(BPlusTree, InsertAndSearch) {
    BPlusTree<string, int> tree(100);
    tree.insert("user1", 123);
    ASSERT_EQ(*tree.search("user1"), 123);
}

TEST(AccountManager, InsertAccount) {
    AccountManager mgr;
    Account acc = {"user1", "password", "User One", 1, false};
    ASSERT_TRUE(mgr.insertAccount(acc));
    ASSERT_FALSE(mgr.insertAccount(acc));  // duplicate
}

TEST(BookManager, SearchByAuthor) {
    BookManager mgr;
    Book book1 = {"ISBN1", "Book 1", "Author A", "keyword1", 10.0, 5, false};
    Book book2 = {"ISBN2", "Book 2", "Author A", "keyword2", 15.0, 3, false};
    mgr.insertBook(book1);
    mgr.insertBook(book2);
    
    SearchCriteria criteria;
    criteria.author = "Author A";
    vector<Book> results = mgr.searchBooks(criteria);
    ASSERT_EQ(results.size(), 2);
}
```

---

## 7. Risk Assessment

### 7.1 Technical Risks

| Risk                          | Probability | Impact | Mitigation                                        |
|-------------------------------|-------------|--------|---------------------------------------------------|
| Index corruption              | Low         | High   | Rebuild from data files (slow but safe)           |
| Insufficient memory (64 MB)   | Low         | High   | Optimize index data structures, lazy loading      |
| Performance degradation       | Medium      | Medium | Profiling, caching, batch operations              |
| File system limitations       | Low         | Medium | Test on target platform, handle errors gracefully |
| Disk full (1024 MB limit)     | Very Low    | High   | Monitor disk usage, garbage collection            |
| Time limit exceeded (10s)     | Low         | High   | Optimize critical paths, avoid full scans         |

### 7.2 Mitigation Strategies

#### Index Corruption

**Problem**: Index file becomes corrupted (e.g., power failure during write)

**Solution**:
1. Detect corruption on load (checksum, magic number)
2. Rebuild indexes from data files (takes ~0.5 seconds for 10K records)
3. Atomic file replacement (write to `.tmp`, rename)

#### Memory Overflow

**Problem**: Indexes exceed 64 MB memory limit

**Solution**:
1. Monitor memory usage during development
2. Optimize index structures (use fixed-size arrays, compact representations)
3. Lazy load indexes if necessary (e.g., keyword index on demand)
4. For 10K records, current design uses ~8-10 MB (safe margin)

#### Performance Issues

**Problem**: Operations exceed 10 second time limit

**Solution**:
1. Profile code to identify bottlenecks
2. Add LRU cache for frequently accessed records
3. Batch file writes (e.g., write every 100 operations)
4. Optimize sorting (use stable_sort, parallel sort if allowed)

#### File Count Limit (20 files)

**Problem**: Accidentally exceed 20 file limit

**Solution**:
1. Current design uses 12 files (8 files buffer)
2. Avoid creating temporary files during runtime
3. Reuse file descriptors, don't leak handles
4. Monitor file count during testing

---

## 8. Alternative Architectures (Considered and Rejected)

### 8.1 Single Large File with Offset Table

**Idea**: Store all data in one file, maintain offset table in memory

**Pros**:
- Simple design
- Only 1 data file + 1 index file

**Cons**:
- Offset table grows large (O(n) memory for n records)
- Difficult to handle variable-size records
- Fragmentation issues
- Slow garbage collection

**Verdict**: Rejected due to memory overhead and complexity

### 8.2 SQLite or Embedded Database

**Idea**: Use SQLite for persistence

**Pros**:
- Proven, battle-tested
- SQL queries for searching
- Transaction support

**Cons**:
- Not allowed (must use C++ only, no external libraries)
- Overkill for this problem
- Opaque file structure (hard to debug)

**Verdict**: Rejected due to constraints

### 8.3 Hash Table Index

**Idea**: Use hash table instead of B+ tree for indexes

**Pros**:
- O(1) average lookup time
- Simpler implementation

**Cons**:
- Doesn't support range queries (not needed here, but good to have)
- Hash collisions can degrade to O(n)
- Difficult to serialize/deserialize efficiently
- B+ tree is more predictable (always O(log n))

**Verdict**: Rejected in favor of B+ tree (more robust)

### 8.4 Memory-Mapped Files

**Idea**: Use mmap() for file I/O

**Pros**:
- Very fast (OS handles caching)
- Simplified code (treat file as memory array)

**Cons**:
- Platform-specific (not portable)
- Complex error handling (segfault on I/O error)
- 64-bit addressing issues on 32-bit systems

**Verdict**: Rejected due to portability concerns

---

## 9. Future Optimizations (If Needed)

### 9.1 LRU Cache for Records

**Idea**: Cache frequently accessed account/book records in memory

```cpp
class LRUCache {
    int capacity;
    list<pair<string, Record>> items;
    map<string, list<pair<string, Record>>::iterator> index;
    
public:
    Record* get(const string& key);
    void put(const string& key, const Record& value);
};
```

**Benefit**: Reduce disk I/O for hot data

**Cost**: ~1-2 MB memory for 1000-record cache

### 9.2 Batch Index Updates

**Idea**: Buffer index updates in memory, flush periodically

```cpp
class IndexUpdateBuffer {
    vector<IndexUpdate> pending;
    
public:
    void addUpdate(const IndexUpdate& update);
    void flush();  // write all pending updates to disk
};
```

**Benefit**: Reduce disk I/O overhead (fewer write calls)

**Cost**: Risk of data loss if crash before flush

### 9.3 Parallel Index Search

**Idea**: Use multiple threads to search different indexes simultaneously

```cpp
vector<Book> BookManager::searchBooks(const SearchCriteria& criteria) {
    vector<future<vector<string>>> futures;
    
    if (!criteria.author.empty()) {
        futures.push_back(async(launch::async, [&]() {
            return authorIndex[criteria.author];
        }));
    }
    
    // Wait for all futures, merge results
    ...
}
```

**Benefit**: Faster multi-criteria search

**Cost**: Complexity, thread safety issues

### 9.4 Compression

**Idea**: Compress index files to reduce disk usage

**Benefit**: Smaller files, faster I/O (if compression is fast)

**Cost**: CPU overhead, complexity

---

## 10. Conclusion

### 10.1 Summary

This architecture provides:
- **Efficient file-based storage** with 12 files (under 20 limit)
- **Fast lookups** via B+ tree indexes (O(log n))
- **Scalability** to tens of thousands of accounts/books
- **Low memory usage** (~8-10 MB for indexes)
- **Predictable performance** (well under 10 second limit)

### 10.2 Key Design Decisions

1. **Separate data and index files**: Allows independent optimization
2. **In-memory B+ tree indexes**: Fast O(log n) lookups, small memory footprint
3. **Multi-value secondary indexes**: Efficient search by author/name/keyword
4. **Append-only logs**: Simple, fast, reliable transaction recording
5. **Fixed-size account records**: Predictable I/O, easy free list management
6. **Variable-size book records**: Space-efficient, handles long keywords

### 10.3 Next Steps

1. **Implement B+ tree class** (core data structure)
2. **Implement FileManager class** (low-level file I/O)
3. **Implement AccountManager** (account CRUD + index)
4. **Implement BookManager** (book CRUD + multi-index)
5. **Implement LogManager** (transaction logging)
6. **Test with small datasets** (100 accounts, 100 books)
7. **Test with large datasets** (10,000 accounts, 10,000 books)
8. **Profile and optimize** (if needed)

### 10.4 Confidence Assessment

| Aspect                | Confidence | Reasoning                                        |
|-----------------------|------------|--------------------------------------------------|
| Correctness           | High       | B+ tree is proven, design is straightforward     |
| Performance           | High       | O(log n) operations, estimated ~5s for 100K ops  |
| Scalability           | High       | Can handle 10× larger datasets with minor tweaks |
| Memory efficiency     | Very High  | Only ~8-10 MB for 10K records                    |
| File limit compliance | Very High  | Uses only 12 files (8 files buffer)              |
| Maintainability       | High       | Modular design, clear separation of concerns     |

**Overall Confidence**: **Very High** (95%)

This architecture is ready for implementation.

---

## Appendix A: B+ Tree Details

### A.1 B+ Tree Node Structure

```cpp
template<typename Key, typename Value>
struct BPlusNode {
    static const int ORDER = 100;  // max keys per node
    
    bool is_leaf;
    int num_keys;
    Key keys[ORDER];
    
    union {
        Value values[ORDER];           // for leaf nodes
        BPlusNode* children[ORDER+1];  // for internal nodes
    };
    
    BPlusNode* next;  // for leaf nodes (linked list)
};
```

### A.2 B+ Tree Operations

#### Insert

```cpp
template<typename Key, typename Value>
bool BPlusTree<Key, Value>::insert(const Key& key, const Value& value) {
    // 1. Search for key
    Node* leaf = findLeaf(key);
    
    // 2. If key exists, return false (duplicate)
    if (leaf->contains(key)) {
        return false;
    }
    
    // 3. If leaf has space, insert
    if (leaf->num_keys < ORDER) {
        leaf->insertKey(key, value);
        return true;
    }
    
    // 4. If leaf is full, split
    Node* newLeaf = splitLeaf(leaf);
    
    // 5. Insert key into appropriate leaf
    if (key < newLeaf->keys[0]) {
        leaf->insertKey(key, value);
    } else {
        newLeaf->insertKey(key, value);
    }
    
    // 6. Propagate split up the tree
    propagateSplit(leaf, newLeaf);
    
    return true;
}
```

#### Search

```cpp
template<typename Key, typename Value>
Value* BPlusTree<Key, Value>::search(const Key& key) {
    // 1. Start at root
    Node* node = root;
    
    // 2. Traverse down to leaf
    while (!node->is_leaf) {
        int i = 0;
        while (i < node->num_keys && key >= node->keys[i]) {
            i++;
        }
        node = node->children[i];
    }
    
    // 3. Search in leaf
    for (int i = 0; i < node->num_keys; i++) {
        if (node->keys[i] == key) {
            return &node->values[i];
        }
    }
    
    return nullptr;  // not found
}
```

---

## Appendix B: File Format Specifications

### B.1 Account Index File (`account_idx.dat`)

```
Offset | Size  | Field
-------|-------|------------------
0      | 8     | Entry count (N)
8      | 24    | Reserved
32     | 32    | Entry[0].user_id
64     | 8     | Entry[0].offset
72     | 32    | Entry[1].user_id
104    | 8     | Entry[1].offset
...
```

### B.2 Transaction Log File (`transactions.log`)

```
Offset | Size  | Field
-------|-------|------------------
0      | 8     | Entry count
8      | 8     | Total income
16     | 8     | Total expenditure
24     | 40    | Reserved
64     | 8     | Entry[0].timestamp
72     | 8     | Entry[0].amount
80     | 16    | Entry[0].type
96     | 24    | Entry[0].isbn
120    | 32    | Entry[0].user_id
152    | 40    | Entry[0].reserved
192    | 8     | Entry[1].timestamp
...
```

---

## Appendix C: Glossary

- **B+ Tree**: Balanced tree data structure optimized for disk-based storage
- **Block**: Fixed-size unit of data (e.g., 4096 bytes)
- **Index**: Data structure for fast lookup (maps key → value)
- **Offset**: Position in a file (in bytes)
- **Serialization**: Converting in-memory data to byte stream for storage
- **Free list**: Linked list of deleted record slots (for reuse)
- **LRU Cache**: Least Recently Used cache (evicts oldest items first)
- **Checkpoint**: Periodic save of in-memory state to disk

---

**End of Document**
