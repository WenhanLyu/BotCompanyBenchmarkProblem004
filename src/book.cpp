#include "book.h"
#include <cctype>
#include <algorithm>
#include <vector>
#include <iomanip>

const std::string BookSystem::BOOKS_FILE = "books.dat";

BookSystem::BookSystem() {
    loadBooks();
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
    std::string line;
    while (std::getline(infile, line)) {
        if (line.empty()) continue;
        
        std::stringstream ss(line);
        std::string isbn, name, author, keyword, priceStr, quantityStr;
        
        // Parse pipe-delimited format
        if (std::getline(ss, isbn, '|') &&
            std::getline(ss, name, '|') &&
            std::getline(ss, author, '|') &&
            std::getline(ss, keyword, '|') &&
            std::getline(ss, priceStr, '|') &&
            std::getline(ss, quantityStr, '|')) {
            
            double price = std::stod(priceStr);
            long long quantity = std::stoll(quantityStr);
            
            Book book(isbn, name, author, keyword, price, quantity);
            books[isbn] = book;
        }
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

// Validation helpers
bool BookSystem::isValidISBN(const std::string& isbn) const {
    if (isbn.empty() || isbn.length() > 20) return false;
    
    // Must be visible ASCII except pipe
    for (char c : isbn) {
        if (c < 33 || c > 126 || c == '|') return false;
    }
    return true;
}

bool BookSystem::isValidName(const std::string& name) const {
    if (name.empty() || name.length() > 60) return false;
    
    // Must be visible ASCII except pipe
    for (char c : name) {
        if (c < 33 || c > 126 || c == '|') return false;
    }
    return true;
}

bool BookSystem::isValidAuthor(const std::string& author) const {
    if (author.empty() || author.length() > 60) return false;
    
    // Must be visible ASCII except pipe
    for (char c : author) {
        if (c < 33 || c > 126 || c == '|') return false;
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
            if (c < 33 || c > 126 || c == '|') return false;
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
        
        // Check if new ISBN already exists (but not same as current)
        if (newISBN != currentISBN && bookExists(newISBN)) {
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
