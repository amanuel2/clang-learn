cmake_minimum_required(VERSION 3.16)
project("classchecker")

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(SOURCE_FILE MatchVisitor.cc)


# Enable compilation database export
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set the path to your custom LLVM installation
set(LLVM_HOME /Users/amanuelbogale/Documents/llvm-project)
set(LLVM_INSTALL ${LLVM_HOME}/install)
set(LLVM_DIR ${LLVM_INSTALL}/lib/cmake/llvm)
set(Clang_DIR ${LLVM_INSTALL}/lib/cmake/clang)
set(LLVM_BUILD ${LLVM_HOME}/build)


# Find LLVM and Clang packages
find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

# Include LLVM and Clang headers
include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})
include_directories(visitor.hh consumer.hh plugin.hh)

# Add LLVM definitions and link directories
add_definitions(${LLVM_DEFINITIONS})
link_directories(${LLVM_LIBRARY_DIRS})

# Set source files
set(SOURCE_FILES checker.cc)

# Add executable
add_library(classchecker MODULE ${SOURCE_FILES})

# Set compile flags
set_target_properties(classchecker PROPERTIES COMPILE_FLAGS "-fno-rtti")

# Link against LLVM and Clang libraries
target_link_libraries(classchecker
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
  TARGET classchecker POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_BINARY_DIR}/compile_commands.json
          ${CMAKE_SOURCE_DIR}/../compile_commands.json
)

# Locate LIT tool
find_program(LIT_CMD 
             NAMES llvm-lit
             PATHS ${LLVM_BUILD}/bin
             NO_DEFAULT_PATH
             REQUIRED)

# Locate FileCheck tool
find_program(FILECHECK_CMD 
             NAMES FileCheck
             PATHS ${LLVM_BUILD}/bin
             NO_DEFAULT_PATH
             REQUIRED)

set(LIT_CONFIG_FILE "@{CMAKE_CURRENT_SOURCE_DIR}/test/lit.site.cfg.py")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/test/lit.site.cfg.py.in"
               "${CMAKE_CURRENT_BINARY_DIR}/test/lit.site.cfg.py"
                @ONLY)


add_custom_target(check-classchecker
                COMMAND ${LIT_CMD} -v ${CMAKE_CURRENT_BINARY_DIR}/test
                COMMENT "Running LIT"
                USES_TERMINAL)