cmake_minimum_required(VERSION 3.16)
project("syntax-check")

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable compilation database export
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set the path to your custom LLVM installation
set(LLVM_HOME /Users/aman/Documents/llvm-project/install)
set(LLVM_DIR ${LLVM_HOME}/lib/cmake/llvm)
set(Clang_DIR ${LLVM_HOME}/lib/cmake/clang)

# Find LLVM and Clang packages
find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

# Include LLVM and Clang headers
include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})

# Add LLVM definitions and link directories
add_definitions(${LLVM_DEFINITIONS})
link_directories(${LLVM_LIBRARY_DIRS})

# Set source files
set(SOURCE_FILES decl.cc)

# Add executable
add_executable(decl ${SOURCE_FILES})

# Set compile flags
set_target_properties(decl PROPERTIES COMPILE_FLAGS "-fno-rtti")

# Link against LLVM and Clang libraries
target_link_libraries(decl
  PRIVATE
  clangTooling
  clangBasic
  clangFrontend
  clangSerialization
  clangASTMatchers
  clangAST
  clangLex
  LLVMSupport

  clangTooling
  clangFrontend
  clangSerialization
  clangDriver
  clangParse
  clangSema
  clangAnalysis
  clangEdit
  clangAST
  clangLex
  clangBasic
)

# Ensure the compile_commands.json is copied to the source directory
add_custom_command(
  TARGET decl POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_BINARY_DIR}/compile_commands.json
          ${CMAKE_SOURCE_DIR}/../compile_commands.json
)