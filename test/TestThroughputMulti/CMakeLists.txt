# Project name
project(testThroughputMulti)

# Require CMake version >=2.8
cmake_minimum_required(VERSION 2.8)

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Appends the cmake/modules path inside the MAKE_MODULE_PATH variable which stores the
# directories of additional CMake modules (ie. MacroOutOfSourceBuild.cmake):
set(CMAKE_MODULE_PATH ${testThroughputMulti_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# Add include directory to include path
include_directories(include)

# Add the source subdirectory
add_subdirectory(src)
 