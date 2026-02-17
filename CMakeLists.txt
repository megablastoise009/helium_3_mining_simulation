# CMake build configuration for the simulation.
cmake_minimum_required(VERSION 3.20)
project(helium_3_mining_simulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)

option(BUILD_TESTING "Build tests" ON)
option(USE_SYSTEM_GTEST "Use a system-installed GoogleTest" OFF)
set(GTEST_SOURCE_DIR "" CACHE PATH "Path to a local GoogleTest source checkout")

add_library(helium_3_mining_sim
    src/simulation.cpp
    src/event_queue.cpp
    src/station.cpp
    src/truck.cpp
)

target_include_directories(helium_3_mining_sim PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(helium_3_mining_sim_cli
    src/main.cpp
)
target_link_libraries(helium_3_mining_sim_cli PRIVATE helium_3_mining_sim)

add_executable(helium_3_mining_sim_sweep_max
    src/sweep_max.cpp
)
target_link_libraries(helium_3_mining_sim_sweep_max PRIVATE helium_3_mining_sim)

if (BUILD_TESTING)
    if (USE_SYSTEM_GTEST)
        find_package(GTest REQUIRED)
    else()
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        if (GTEST_SOURCE_DIR)
            add_subdirectory(${GTEST_SOURCE_DIR} ${CMAKE_BINARY_DIR}/_deps/googletest-build)
        elseif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest/CMakeLists.txt)
            add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest
                ${CMAKE_BINARY_DIR}/_deps/googletest-build)
        else()
            include(FetchContent)
            FetchContent_Declare(
                googletest
                URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
            )
            FetchContent_MakeAvailable(googletest)
        endif()
    endif()

    enable_testing()
    add_executable(helium_3_mining_sim_tests
        tests/test_truck.cpp
        tests/test_station.cpp
        tests/test_event_queue.cpp
        tests/test_simulation.cpp
        tests/test_sweep.cpp
    )
    target_include_directories(helium_3_mining_sim_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    target_link_libraries(helium_3_mining_sim_tests PRIVATE helium_3_mining_sim GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(helium_3_mining_sim_tests)
endif()
