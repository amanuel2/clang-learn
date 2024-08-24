cmake_minimum_required(VERSION 3.16)
project("libs")

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(SOURCE_FILE MatchVisitor.cc)


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
include_directories(cast.hh maps.hh string.hh ptrs.hh diags.hh)

# Add LLVM definitions and link directories
add_definitions(${LLVM_DEFINITIONS})
link_directories(${LLVM_LIBRARY_DIRS})

# Set source files
set(SOURCE_FILES libs.cc)

# Add executable
add_executable(libs ${SOURCE_FILES})

# Set compile flags
set_target_properties(libs PROPERTIES COMPILE_FLAGS "-fno-rtti")

# Link against LLVM and Clang libraries
target_link_libraries(libs
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
  TARGET libs POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_BINARY_DIR}/compile_commands.json
          ${CMAKE_SOURCE_DIR}/../compile_commands.json
)

# add_custom_command(
#   OUTPUT ${SOURCE_FILE}.preprocessed
#   COMMAND ${CMAKE_CXX_COMPILER} -E -nostdinc++ -isystem${LLVM_HOME}/include/c++/v1 -I${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE} > ${SOURCE_FILE}.preprocessed 
#   DEPENDS ${SOURCE_FILE} 
#   COMMENT "Preprocessing ${SOURCE_FILE}"
# )

# add_custom_target(preprocess ALL DEPENDS ${SOURCE_FILE}.preprocessed)