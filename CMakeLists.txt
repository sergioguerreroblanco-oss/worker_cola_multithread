cmake_minimum_required(VERSION 3.21)

project(worker_cola_multithread 
    VERSION 1.0.0
    DESCRIPTION "Thread-safe queue and worker pool in modern C++14"
    HOMEPAGE_URL "https://github.com/sergioguerreroblanco-oss/worker_cola_multithread"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

# -----------------------------------------------------------
# Core library
# -----------------------------------------------------------
add_library(core STATIC
    src/logger.cpp
    src/worker_pool.cpp
)

target_include_directories(core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/optional-lite/include
)

if(MSVC)
    target_compile_options(core PRIVATE /W4 /permissive- /wd26495)
else()
    target_compile_options(core PRIVATE -Wall -Wextra -Wpedantic)
endif()

# -----------------------------------------------------------
# Main executable
# -----------------------------------------------------------
add_executable(cola_worker src/main.cpp)
target_link_libraries(cola_worker PRIVATE core)

# -----------------------------------------------------------
# Tests
# -----------------------------------------------------------
if(BUILD_TESTING)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(tests tests/test_main.cpp)
    target_link_libraries(tests PRIVATE core gtest_main)

    include(GoogleTest)
    gtest_discover_tests(tests)
endif()
