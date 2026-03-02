#ifndef BOOK_H
#define BOOK_H

#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>

// Book structure - stored in file and memory
struct Book {
    std::string ISBN;        // 1-20 chars, visible ASCII except pipe
    std::string name;        // 1-60 chars, visible ASCII except pipe (quoted in show output)
    std::string author;      // 1-60 chars, visible ASCII except pipe (quoted in show output)
    std::string keyword;     // pipe-separated keywords, each 1-60 chars, no duplicates
    double price;            // Price with 2 decimal places
    long long quantity;      // Inventory quantity (non-negative)
    
    Book() : price(0.0), quantity(0) {}
    Book(const std::string& isbn)
        : ISBN(isbn), name(""), author(""), keyword(""), price(0.0), quantity(0) {}
    Book(const std::string& isbn, const std::string& n, const std::string& auth,
         const std::string& kw, double p, long long qty)
        : ISBN(isbn), name(n), author(auth), keyword(kw), price(p), quantity(qty) {}
};

// Finance record structure - tracks income and expenses
struct FinanceRecord {
    enum Type { INCOME, EXPENSE };
    Type type;
    double amount;
    
    FinanceRecord(Type t, double amt) : type(t), amount(amt) {}
};

// Book system class - manages books, indices, and file persistence
class BookSystem {
private:
    static const std::string BOOKS_FILE;
    static const std::string FINANCE_FILE;
    
    std::map<std::string, Book> books;  // ISBN -> Book (in-memory, primary index)
    std::vector<FinanceRecord> financeRecords;  // Transaction history
    
    // File I/O operations
    void loadBooks();
    void saveBook(const Book& book);
    void updateBook(const Book& book);
    void loadFinance();
    void saveFinance();
    
    // Helper functions
    bool isValidISBN(const std::string& isbn) const;
    bool isValidName(const std::string& name) const;
    bool isValidAuthor(const std::string& author) const;
    bool isValidKeyword(const std::string& keyword) const;
    bool bookExists(const std::string& ISBN) const;
    
public:
    BookSystem();
    
    // Book management commands
    bool selectBook(const std::string& ISBN);
    bool createBook(const std::string& ISBN);
    Book* getBook(const std::string& ISBN);
    const Book* getBook(const std::string& ISBN) const;
    bool modifyBook(const std::string& currentISBN, const std::string& newISBN,
                    const std::string& name, const std::string& author,
                    const std::string& keyword, double price);
    
    // Query operations
    std::vector<Book> getAllBooks() const;
    std::vector<Book> findByISBN(const std::string& ISBN) const;
    std::vector<Book> findByName(const std::string& name) const;
    std::vector<Book> findByAuthor(const std::string& author) const;
    std::vector<Book> findByKeyword(const std::string& keyword) const;
    
    // Transaction operations
    double buyBook(const std::string& ISBN, long long quantity);  // Returns total cost, -1 on failure
    bool importBook(const std::string& ISBN, long long quantity, double totalCost);
    
    // Finance operations
    std::string showFinance(int count = 0) const;  // Returns formatted finance report
};

#endif // BOOK_H
