cmake_minimum_required(VERSION 3.21)

project(JDI VERSION 1.1 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 17)

# SDL2
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)


# Include headers
include_directories(
    ${SDL2_INCLUDE_DIR}
    ${SDL2_TTF_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/include
)

# Gather all source files
file(GLOB SOURCES src/*.cpp)

# The JDI library!
# (Do we need to add the type here?)
add_library(jdi_static STATIC ${SOURCES})
target_include_directories(jdi_static PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_library(jdi SHARED ${SOURCES})
target_include_directories(jdi PUBLIC ${PROJECT_SOURCE_DIR}/include)

## ====================
## Testing follows here
## ====================

file(GLOB TEST_SOURCES src/test/*.cpp)

foreach(test_src ${TEST_SOURCES})
    # Extract the filenames without extension
    get_filename_component(test_name ${test_src} NAME_WE)

    # Define an executable target for each test
    add_executable(${test_name} ${test_src})
    target_link_libraries(${test_name} PRIVATE jdi SDL2_image SDL2_mixer SDL2_ttf::SDL2_ttf ${SDL2_LIBRARIES} )
endforeach()

enable_testing()
add_test(NAME EngineTest COMMAND engine_test WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
