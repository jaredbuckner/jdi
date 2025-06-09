cmake_minimum_required(VERSION 3.21)

project(JDI VERSION 1.1 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 17)

# SDL2
find_package(SDL2 REQUIRED)

# Include headers
include_directories(
    ${SDL2_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/include
)

link_directories(
    ${SDL_LIB_DIR}
)

# Gather all source files
file(GLOB SOURCES src/*.cpp)

# The JDI library!
# (Do we need to add the type here?)
add_library(jdi_static STATIC ${SOURCES})
target_include_directories(jdi_static PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_library(jdi SHARED ${SOURCES})
target_include_directories(jdi PUBLIC ${PROJECT_SOURCE_DIR}/include)

