cmake_minimum_required(VERSION 3.19)

# For FetchContent (CMP0135 warning)
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

project(worker_cola_multithread 
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTING "Enable tests" OFF)

add_library(core STATIC
    src/logger.cpp
    src/worker_pool.cpp
)
target_include_directories(core PUBLIC include)
target_include_directories(core PUBLIC third_party/optional-lite/include)

# Configure Compiler Flags
if(MSVC)
    target_compile_options(core PRIVATE /W4 /permissive-)
else()
    target_compile_options(core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(cola_worker src/main.cpp)
target_link_libraries(cola_worker PRIVATE core)

if(BUILD_TESTING)
    include(CTest)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    target_include_directories(gtest       SYSTEM PRIVATE ${gtest_SOURCE_DIR}/include)
    target_include_directories(gtest_main  SYSTEM PRIVATE ${gtest_SOURCE_DIR}/include)
    target_include_directories(gmock       SYSTEM PRIVATE ${gtest_SOURCE_DIR}/include)
    target_include_directories(gmock_main  SYSTEM PRIVATE ${gtest_SOURCE_DIR}/include)

    add_executable(tests tests/test_main.cpp)
    target_link_libraries(tests PRIVATE core gtest_main)

    include(GoogleTest)
    gtest_discover_tests(tests)
endif()
