cmake_minimum_required(VERSION 3.1)
project(EditPropagation)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

option(JSONCPP_WITH_TESTS OFF)
add_subdirectory(externals/jsoncpp)
include_directories(externals/jsoncpp/include)

add_subdirectory(externals/eigen)
include_directories(externals/eigen)

include_directories(externals/nanoflann/include)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O2 -O3 -Wall")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(src)
