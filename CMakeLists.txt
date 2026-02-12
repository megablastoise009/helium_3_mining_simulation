cmake_minimum_required(VERSION 3.20)
project(vast_interview LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTING "Build tests" ON)

add_library(vast_sim
    src/simulation.cpp
)

target_include_directories(vast_sim PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(vast_sim_cli
    src/main.cpp
)
target_link_libraries(vast_sim_cli PRIVATE vast_sim)

if (BUILD_TESTING)
    enable_testing()
    add_executable(vast_sim_tests
        tests/test_main.cpp
        tests/test_truck.cpp
        tests/test_station.cpp
        tests/test_event_queue.cpp
        tests/test_simulation.cpp
        tests/test_sweep.cpp
    )
    target_include_directories(vast_sim_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    target_link_libraries(vast_sim_tests PRIVATE vast_sim)
    add_test(NAME vast_sim_tests COMMAND vast_sim_tests)
endif()
