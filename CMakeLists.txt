cmake_minimum_required(VERSION 3.10)
project(BookstoreSystem)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/main.cpp
    src/account.cpp
    src/book.cpp
)

# Create executable named 'code'
add_executable(code ${SOURCES})

# Include directories
target_include_directories(code PRIVATE src)
