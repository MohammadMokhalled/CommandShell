cmake_minimum_required(VERSION 3.14)

project(desktop-sample LANGUAGES CXX)

add_executable(desktop-sample
    src/main.cpp
)

target_link_libraries(desktop-sample PRIVATE CommandShell)
target_compile_features(desktop-sample PRIVATE cxx_std_17)

if(MSVC)
    string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    target_compile_options(desktop-sample PRIVATE /W4)
else()
    target_compile_options(desktop-sample PRIVATE -Wall -Wextra -Wpedantic)
endif()

