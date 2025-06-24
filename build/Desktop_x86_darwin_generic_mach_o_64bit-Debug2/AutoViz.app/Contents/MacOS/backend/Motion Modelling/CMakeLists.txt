cmake_minimum_required(VERSION 3.16)
project(AutoVizBackend)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add include directories
include_directories(
    "${CMAKE_SOURCE_DIR}/external"
    "${CMAKE_SOURCE_DIR}"
)

add_executable(motionmodel motionmodel.cpp ${BACKEND_SOURCES})