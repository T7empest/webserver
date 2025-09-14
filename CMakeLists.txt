cmake_minimum_required(VERSION 4.0)
project(webserver)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SRC_FILES src/*.cc)
add_executable(webserver ${SRC_FILES})

include_directories(webserver
        src/core
        src/http
        src/
)