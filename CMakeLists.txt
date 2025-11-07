cmake_minimum_required(VERSION 3.14)

project(CommandShell VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# For MSVC, ensure proper standard flag
if(MSVC)
    add_compile_options(/std:c++17)
endif()

# Export compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Option to build tests
option(BUILD_TESTS "Build the tests" ON)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/inc)

# Source files
set(SOURCES
    src/CommandShell.cpp
    src/CommandShellIO.cpp
)

# Header files
set(HEADERS
    inc/CommandShell.hpp
    inc/CommandShellIO.hpp
    inc/CommandTypes.hpp
)

# Create library
add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

# Set include directories for the library
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<INSTALL_INTERFACE:inc>
)

# Compiler-specific options
if(MSVC)
    # MSVC specific flags
    # Remove default warning level and add /W4
    string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
    # Disable specific warnings that are problematic
    target_compile_options(${PROJECT_NAME} PRIVATE /wd4624)  # Disable C4624 (deleted destructor warning)
else()
    # GCC/Clang flags
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# Build tests if enabled
if(BUILD_TESTS)
    enable_testing()
    
    # FetchContent for Google Test
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0
    )
    
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    
    FetchContent_MakeAvailable(googletest)
    
    # Test executable
    add_executable(${PROJECT_NAME}_tests
        tests/CommandShellIOTests.cpp
        tests/CommandShellTests.cpp
    )
    
    # Link test executable with library and gtest
    target_link_libraries(${PROJECT_NAME}_tests
        PRIVATE
            ${PROJECT_NAME}
            gtest_main
            gtest
    )
    
    # Include GoogleTest module
    include(GoogleTest)
    gtest_discover_tests(${PROJECT_NAME}_tests)
    
    # Add custom target to run tests
    add_custom_target(run_tests
        COMMAND ${PROJECT_NAME}_tests
        DEPENDS ${PROJECT_NAME}_tests
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running tests..."
    )
endif()

# Installation rules
install(TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY inc/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)
