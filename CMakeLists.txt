cmake_minimum_required(VERSION 3.20)
project(webserver LANGUAGES CXX)

# Make sure we use at least C++20 for std::jthread
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexperimental-library")

file(GLOB_RECURSE SRC_FILES src/*.cc)
add_executable(webserver ${SRC_FILES}
        src/http/constants.h)


target_include_directories(webserver PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/http
)

# Ensure threads are linked
find_package(Threads REQUIRED)
target_link_libraries(webserver PRIVATE Threads::Threads)

# Alternatively, enforce the standard per target (in addition to the cache/global setting)
target_compile_features(webserver PRIVATE cxx_std_20)