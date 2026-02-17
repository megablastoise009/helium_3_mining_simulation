# CMake build configuration for the simulation.
cmake_minimum_required(VERSION 3.20)
project(vast_interview LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)

option(BUILD_TESTING "Build tests" ON)

add_library(vast_sim
    src/simulation.cpp
    src/event_queue.cpp
    src/station.cpp
    src/truck.cpp
)

target_include_directories(vast_sim PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(vast_sim_cli
    src/main.cpp
)
target_link_libraries(vast_sim_cli PRIVATE vast_sim)

add_executable(vast_sim_sweep_max
    src/sweep_max.cpp
)
target_link_libraries(vast_sim_sweep_max PRIVATE vast_sim)

if (BUILD_TESTING)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    add_executable(vast_sim_tests
        tests/test_truck.cpp
        tests/test_station.cpp
        tests/test_event_queue.cpp
        tests/test_simulation.cpp
        tests/test_sweep.cpp
    )
    target_include_directories(vast_sim_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    target_link_libraries(vast_sim_tests PRIVATE vast_sim GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(vast_sim_tests)
endif()
