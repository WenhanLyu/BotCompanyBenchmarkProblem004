cmake_minimum_required(VERSION 3.10)
project(BookstoreSystem)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directory to project root for the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Collect all source files
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
)

# If no source files exist yet, create a placeholder main
if(NOT SOURCES)
    set(SOURCES "${CMAKE_SOURCE_DIR}/src/main.cpp")
endif()

# Create executable named 'code' in project root
add_executable(code ${SOURCES})

# Include directories
target_include_directories(code PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
)

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(code PRIVATE -Wall -Wextra)
endif()
