#include "book.h"
#include <cctype>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <iostream>

const std::string BookSystem::BOOKS_FILE = "books.dat";
const std::string BookSystem::FINANCE_FILE = "transactions.dat";

BookSystem::BookSystem() {
    loadBooks();
    loadFinance();
}

// Load books from file
void BookSystem::loadBooks() {
    std::ifstream infile(BOOKS_FILE);
    
    if (!infile.is_open()) {
        // File doesn't exist - no books to load
        return;
    }
    
    // Read books from file
    // Format: ISBN|Name|Author|Keyword|Price|Quantity
    // Note: Keyword field can contain pipes, so we parse from both ends
    std::string line;
    while (std::getline(infile, line)) {
        if (line.empty()) continue;
        
        // Find the last two pipe positions (for Price and Quantity)
        size_t lastPipe = line.rfind('|');
        if (lastPipe == std::string::npos) continue;
        
        size_t secondLastPipe = line.rfind('|', lastPipe - 1);
        if (secondLastPipe == std::string::npos) continue;
        
        // Extract Price and Quantity from the end
        std::string quantityStr = line.substr(lastPipe + 1);
        std::string priceStr = line.substr(secondLastPipe + 1, lastPipe - secondLastPipe - 1);
        
        // Parse the first part (ISBN|Name|Author|Keyword)
        std::string firstPart = line.substr(0, secondLastPipe);
        
        // Find first 3 pipes for ISBN, Name, Author
        size_t firstPipe = firstPart.find('|');
        if (firstPipe == std::string::npos) continue;
        
        size_t secondPipe = firstPart.find('|', firstPipe + 1);
        if (secondPipe == std::string::npos) continue;
        
        size_t thirdPipe = firstPart.find('|', secondPipe + 1);
        if (thirdPipe == std::string::npos) continue;
        
        // Extract fields
        std::string isbn = firstPart.substr(0, firstPipe);
        std::string name = firstPart.substr(firstPipe + 1, secondPipe - firstPipe - 1);
        std::string author = firstPart.substr(secondPipe + 1, thirdPipe - secondPipe - 1);
        std::string keyword = firstPart.substr(thirdPipe + 1);
        
        // Convert price and quantity
        double price = std::stod(priceStr);
        long long quantity = std::stoll(quantityStr);
        
        Book book(isbn, name, author, keyword, price, quantity);
        books[isbn] = book;
    }
    
    infile.close();
}

// Append new book to file
void BookSystem::saveBook(const Book& book) {
    std::ofstream outfile(BOOKS_FILE, std::ios::app);
    if (!outfile.is_open()) return;
    
    // Format: ISBN|Name|Author|Keyword|Price|Quantity
    outfile << book.ISBN << "|" 
            << book.name << "|" 
            << book.author << "|" 
            << book.keyword << "|" 
            << std::fixed << std::setprecision(2) << book.price << "|" 
            << book.quantity << "\n";
    
    outfile.flush();
    outfile.close();
}

// Update existing book in file (rewrite entire file)
void BookSystem::updateBook(const Book& book) {
    // Read all books from file
    std::ifstream infile(BOOKS_FILE);
    std::vector<std::string> lines;
    std::string line;
    
    while (std::getline(infile, line)) {
        if (line.empty()) continue;
        
        std::stringstream ss(line);
        std::string isbn;
        std::getline(ss, isbn, '|');
        
        // If this is the book we're updating, replace it
        if (isbn == book.ISBN) {
            std::stringstream newLine;
            newLine << book.ISBN << "|" 
                    << book.name << "|" 
                    << book.author << "|" 
                    << book.keyword << "|" 
                    << std::fixed << std::setprecision(2) << book.price << "|" 
                    << book.quantity;
            lines.push_back(newLine.str());
        } else {
            lines.push_back(line);
        }
    }
    infile.close();
    
    // Rewrite file
    std::ofstream outfile(BOOKS_FILE);
    if (!outfile.is_open()) return;
    
    for (const auto& l : lines) {
        outfile << l << "\n";
    }
    
    outfile.flush();
    outfile.close();
}

// Load finance records from file
void BookSystem::loadFinance() {
    std::ifstream infile(FINANCE_FILE);
    
    if (!infile.is_open()) {
        // File doesn't exist - no finance records to load
        return;
    }
    
    // Read finance records from file
    // Format: type|amount
    // type: 0 = INCOME, 1 = EXPENSE
    std::string line;
    while (std::getline(infile, line)) {
        if (line.empty()) continue;
        
        size_t pipePos = line.find('|');
        if (pipePos == std::string::npos) continue;
        
        std::string typeStr = line.substr(0, pipePos);
        std::string amountStr = line.substr(pipePos + 1);
        
        int typeInt = std::stoi(typeStr);
        double amount = std::stod(amountStr);
        
        FinanceRecord::Type type = (typeInt == 0) ? FinanceRecord::INCOME : FinanceRecord::EXPENSE;
        financeRecords.push_back(FinanceRecord(type, amount));
    }
    
    infile.close();
}

// Save all finance records to file (rewrite entire file)
void BookSystem::saveFinance() {
    std::ofstream outfile(FINANCE_FILE);
    if (!outfile.is_open()) return;
    
    // Write all finance records
    // Format: type|amount
    // type: 0 = INCOME, 1 = EXPENSE
    for (const auto& record : financeRecords) {
        int typeInt = (record.type == FinanceRecord::INCOME) ? 0 : 1;
        outfile << typeInt << "|" 
                << std::fixed << std::setprecision(2) << record.amount << "\n";
    }
    
    outfile.flush();
    outfile.close();
}

// Validation helpers
bool BookSystem::isValidISBN(const std::string& isbn) const {
    if (isbn.empty() || isbn.length() > 20) return false;
    
    // Must be visible ASCII except pipe and double quote
    for (char c : isbn) {
        if (c < 33 || c > 126 || c == '|' || c == '"') return false;
    }
    return true;
}

bool BookSystem::isValidName(const std::string& name) const {
    if (name.empty() || name.length() > 60) return false;
    
    // Must be visible ASCII except pipe and double quote
    for (char c : name) {
        if (c < 33 || c > 126 || c == '|' || c == '"') return false;
    }
    return true;
}

bool BookSystem::isValidAuthor(const std::string& author) const {
    if (author.empty() || author.length() > 60) return false;
    
    // Must be visible ASCII except pipe and double quote
    for (char c : author) {
        if (c < 33 || c > 126 || c == '|' || c == '"') return false;
    }
    return true;
}

bool BookSystem::isValidKeyword(const std::string& keyword) const {
    if (keyword.empty() || keyword.length() > 60) return false;
    
    // Must be visible ASCII except pipe (pipe is used as separator)
    // Keywords are pipe-separated, each segment 1-60 chars
    std::vector<std::string> segments;
    std::stringstream ss(keyword);
    std::string segment;
    
    while (std::getline(ss, segment, '|')) {
        if (segment.empty() || segment.length() > 60) return false;
        
        // Check for duplicates
        if (std::find(segments.begin(), segments.end(), segment) != segments.end()) {
            return false;
        }
        segments.push_back(segment);
        
        // Check characters
        for (char c : segment) {
            if (c < 33 || c > 126 || c == '|' || c == '"') return false;
        }
    }
    
    return !segments.empty();
}

bool BookSystem::bookExists(const std::string& ISBN) const {
    return books.find(ISBN) != books.end();
}

// Book management operations
bool BookSystem::selectBook(const std::string& ISBN) {
    if (!isValidISBN(ISBN)) return false;
    return bookExists(ISBN);
}

bool BookSystem::createBook(const std::string& ISBN) {
    if (!isValidISBN(ISBN)) return false;
    
    // Don't create if already exists
    if (bookExists(ISBN)) return false;
    
    // Create new book with ISBN only
    Book book(ISBN);
    books[ISBN] = book;
    saveBook(book);
    
    return true;
}

Book* BookSystem::getBook(const std::string& ISBN) {
    if (!bookExists(ISBN)) return nullptr;
    return &books[ISBN];
}

const Book* BookSystem::getBook(const std::string& ISBN) const {
    auto it = books.find(ISBN);
    if (it == books.end()) return nullptr;
    return &(it->second);
}

std::vector<Book> BookSystem::getAllBooks() const {
    std::vector<Book> result;
    for (const auto& pair : books) {
        result.push_back(pair.second);
    }
    return result;
}

// Find books by ISBN (exact match)
std::vector<Book> BookSystem::findByISBN(const std::string& ISBN) const {
    std::vector<Book> result;
    const Book* book = getBook(ISBN);
    if (book) {
        result.push_back(*book);
    }
    return result;
}

// Find books by name (exact match)
std::vector<Book> BookSystem::findByName(const std::string& name) const {
    std::vector<Book> result;
    for (const auto& pair : books) {
        if (pair.second.name == name) {
            result.push_back(pair.second);
        }
    }
    return result;
}

// Find books by author (exact match)
std::vector<Book> BookSystem::findByAuthor(const std::string& author) const {
    std::vector<Book> result;
    for (const auto& pair : books) {
        if (pair.second.author == author) {
            result.push_back(pair.second);
        }
    }
    return result;
}

// Find books by keyword (segment match - keyword contains this segment)
std::vector<Book> BookSystem::findByKeyword(const std::string& keyword) const {
    std::vector<Book> result;
    for (const auto& pair : books) {
        // Check if the keyword appears as a segment in the book's keywords
        // Keywords are pipe-separated
        if (pair.second.keyword.empty()) continue;
        
        std::stringstream ss(pair.second.keyword);
        std::string segment;
        bool found = false;
        
        while (std::getline(ss, segment, '|')) {
            if (segment == keyword) {
                found = true;
                break;
            }
        }
        
        if (found) {
            result.push_back(pair.second);
        }
    }
    return result;
}

// Modify book attributes
// Parameters with empty strings mean "don't modify this field"
// Returns false if validation fails or book doesn't exist
bool BookSystem::modifyBook(const std::string& currentISBN, const std::string& newISBN,
                             const std::string& name, const std::string& author,
                             const std::string& keyword, double price) {
    // Book must exist
    if (!bookExists(currentISBN)) return false;
    
    Book* book = getBook(currentISBN);
    if (!book) return false;
    
    // Validate new ISBN if provided
    if (!newISBN.empty()) {
        if (!isValidISBN(newISBN)) return false;
        
        // Cannot change ISBN to its original ISBN (per specification)
        if (newISBN == currentISBN) {
            return false;
        }
        
        // Check if new ISBN already exists
        if (bookExists(newISBN)) {
            return false;
        }
    }
    
    // Validate other fields if provided
    if (!name.empty() && !isValidName(name)) return false;
    if (!author.empty() && !isValidAuthor(author)) return false;
    if (!keyword.empty() && !isValidKeyword(keyword)) return false;
    if (price >= 0 && price > 1e13) return false; // Price limit check
    
    // Handle ISBN modification (requires removing old entry and adding new one)
    if (!newISBN.empty() && newISBN != currentISBN) {
        // Create a copy of the book with new ISBN
        Book newBook = *book;
        newBook.ISBN = newISBN;
        
        // Apply other modifications
        if (!name.empty()) newBook.name = name;
        if (!author.empty()) newBook.author = author;
        if (!keyword.empty()) newBook.keyword = keyword;
        if (price >= 0) newBook.price = price;
        
        // Remove old book from memory
        books.erase(currentISBN);
        
        // Add new book to memory
        books[newISBN] = newBook;
        
        // Update file - need to rewrite entire file
        std::vector<Book> allBooks;
        for (const auto& pair : books) {
            allBooks.push_back(pair.second);
        }
        
        std::ofstream outfile(BOOKS_FILE);
        if (!outfile.is_open()) return false;
        
        for (const auto& b : allBooks) {
            outfile << b.ISBN << "|" 
                    << b.name << "|" 
                    << b.author << "|" 
                    << b.keyword << "|" 
                    << std::fixed << std::setprecision(2) << b.price << "|" 
                    << b.quantity << "\n";
        }
        
        outfile.flush();
        outfile.close();
    } else {
        // No ISBN change - just update other fields
        if (!name.empty()) book->name = name;
        if (!author.empty()) book->author = author;
        if (!keyword.empty()) book->keyword = keyword;
        if (price >= 0) book->price = price;
        
        // Update in file
        updateBook(*book);
    }
    
    return true;
}

// Buy book - decrease quantity and return total cost
// Returns -1.0 if book doesn't exist, quantity invalid, or insufficient stock
double BookSystem::buyBook(const std::string& ISBN, long long quantity) {
    // Validate quantity
    if (quantity <= 0) {
        return -1.0;
    }
    
    // Check if book exists
    auto it = books.find(ISBN);
    if (it == books.end()) {
        return -1.0;
    }
    
    Book* book = &(it->second);
    
    // Check if sufficient stock
    if (book->quantity < quantity) {
        return -1.0;
    }
    
    // Calculate total cost
    double totalCost = book->price * quantity;
    
    // Decrease quantity
    book->quantity -= quantity;
    
    // Update in file
    updateBook(*book);
    
    // Record income transaction
    financeRecords.push_back(FinanceRecord(FinanceRecord::INCOME, totalCost));
    
    // Save finance records to file
    saveFinance();
    
    return totalCost;
}

// Import book - increase quantity for selected book
// Returns false if ISBN doesn't match selected book, quantity invalid, or cost invalid
bool BookSystem::importBook(const std::string& ISBN, long long quantity, double totalCost) {
    // Validate quantity
    if (quantity <= 0) {
        return false;
    }
    
    // Validate totalCost
    if (totalCost <= 0) {
        return false;
    }
    
    // Check if book exists
    auto it = books.find(ISBN);
    if (it == books.end()) {
        return false;
    }
    
    Book* book = &(it->second);
    
    // Increase quantity
    book->quantity += quantity;
    
    // Update in file
    updateBook(*book);
    
    // Record expense transaction
    financeRecords.push_back(FinanceRecord(FinanceRecord::EXPENSE, totalCost));
    
    // Save finance records to file
    saveFinance();
    
    return true;
}

// Show finance - calculate income and expenses from transactions
// count = 0: show all transactions
// count > 0: show last N transactions
std::string BookSystem::showFinance(int count) const {
    double totalIncome = 0.0;
    double totalExpense = 0.0;
    
    // Determine which transactions to include
    int startIdx = 0;
    int totalRecords = financeRecords.size();
    
    if (count > 0 && count < totalRecords) {
        // Show last N transactions
        startIdx = totalRecords - count;
    }
    // If count == 0 or count >= total records, include all
    
    // Calculate totals from selected range
    for (int i = startIdx; i < totalRecords; i++) {
        if (financeRecords[i].type == FinanceRecord::INCOME) {
            totalIncome += financeRecords[i].amount;
        } else {
            totalExpense += financeRecords[i].amount;
        }
    }
    
    // Format output: "+ <income> - <expense>"
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "+ " << totalIncome << " - " << totalExpense;
    
    return ss.str();
}

// Generate detailed finance report and output to stdout
void BookSystem::reportFinance() const {
    double totalIncome = 0.0;
    double totalExpense = 0.0;
    int transactionCount = financeRecords.size();
    
    // Calculate totals from all transactions
    for (const auto& record : financeRecords) {
        if (record.type == FinanceRecord::INCOME) {
            totalIncome += record.amount;
        } else {
            totalExpense += record.amount;
        }
    }
    
    double netIncome = totalIncome - totalExpense;
    
    // Output report in readable format
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "=== Financial Report ===" << std::endl;
    std::cout << "Total Income:   " << totalIncome << std::endl;
    std::cout << "Total Expense:  " << totalExpense << std::endl;
    std::cout << "Net Income:     " << netIncome << std::endl;
    std::cout << "Transaction Count: " << transactionCount << std::endl;
}
